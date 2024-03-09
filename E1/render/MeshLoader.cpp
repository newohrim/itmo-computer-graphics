#include "MeshLoader.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>
#include <vector>
#include <cassert>
#include <functional>

#include "components/CompositeComponent.h"
#include "components/MeshComponent.h"
#include "render/GeometryData.h"
#include "core/Game.h"
#include "render/Renderer.h"
#include "render/RenderUtils.h"

#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

void CopyNodesWithMeshes(const aiScene* scene, aiNode* node, Compositer* targetParent, const aiMatrix4x4& accTransform, Game* game);
void AddMesh(const aiScene* scene, int meshIdx, MeshComponent* meshComp);

bool MeshLoader::LoadMesh(const std::string& path, CompositeComponent* parent)
{
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_ConvertToLeftHanded);

    // If the import failed, report it
    if (nullptr == scene) {
        //DoTheErrorLogging(importer.GetErrorString());
        std::cout << importer.GetErrorString() << '\n';
        return false;
    }

    // Now we can access the file's contents.
    //DoTheSceneProcessing(scene);
    CopyNodesWithMeshes(scene, scene->mRootNode, parent, aiMatrix4x4{}, parent->GetGame());

    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

void CopyNodesWithMeshes(const aiScene* scene, aiNode* node, Compositer* targetParent, const aiMatrix4x4& accTransform, Game* game) {
    Compositer* parent;
    aiMatrix4x4 transform;

    // if node has meshes, create a new scene object for it
    if (node->mNumMeshes > 0) {
        // copy the meshes
        CompositeComponent* newObject = new CompositeComponent(game, targetParent);
        for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
            AddMesh(scene, i, new MeshComponent(game, targetParent));
        }

        // the new object is the parent for all child nodes
        parent = newObject;
        //  transform.SetUnity();  wtf is this???
        aiVector3D pos;
        aiVector3D rotAxis;
        ai_real angle;
        aiVector3D scale;
        transform.Decompose(scale, rotAxis, angle, pos);
        newObject->SetPosition(*reinterpret_cast<Math::Vector3*>(&pos));
        if (rotAxis.SquareLength() > 0.99f) {
            newObject->SetRotation(Math::Quaternion::CreateFromAxisAngle(
                *reinterpret_cast<Math::Vector3*>(&rotAxis), angle));
        }
        newObject->SetPosition(*reinterpret_cast<Math::Vector3*>(&scale));
    }
    else {
        // if no meshes, skip the node, but keep its transformation
        parent = targetParent;
        transform = node->mTransformation * accTransform;
    }

    // continue for all child nodes
    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        aiNode* child = node->mChildren[i];
        CopyNodesWithMeshes(scene, child, parent, transform, game);
    }
}

struct MeshData {
    std::vector<float> vertFloats;
    std::vector<uint32_t> indices;
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputs;
    uint32_t stride = 0;
};

MeshData GatherMeshData(const aiScene* scene, int meshIdx) 
{
    MeshData res;
    std::vector<std::function<void(int)>> pipeline;
    auto addVector = [&res](const aiVector3D& v) {
        res.vertFloats.push_back(v.x);
        res.vertFloats.push_back(v.y);
        res.vertFloats.push_back(v.z);
    };
    auto addInput3 = [&res](const char* channel) {
        res.stride += sizeof(float) * 3;
        res.inputs.push_back({
            channel,
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        });
    };

    const aiMesh* mesh = scene->mMeshes[meshIdx];
    if (!mesh->HasPositions()) {
        return res;
    }
    addInput3("POSITION");

    uint32_t floatsNum = mesh->mNumVertices * 3;
    if (floatsNum == 0) {
        return res;
    }
    if (mesh->HasNormals()) {
        floatsNum += mesh->mNumVertices * 3;
        pipeline.push_back([&](int idx) { addVector(mesh->mNormals[idx]); });
        addInput3("NORMAL");
    }
    static constexpr int TEXCOORD_IDX = 0;  // TODO: support several uv channels?
    if (mesh->GetNumUVChannels() > 0 && mesh->HasTextureCoords(TEXCOORD_IDX)) {
        const uint32_t uvCompNum = mesh->mNumUVComponents[TEXCOORD_IDX];
        floatsNum += mesh->mNumVertices * uvCompNum;
        pipeline.push_back([&](int idx) {
            for (uint32_t i = 0; i < uvCompNum; ++i) {
                res.vertFloats.push_back(mesh->mTextureCoords[TEXCOORD_IDX][idx][i]);
            }
        });
        DXGI_FORMAT uvFormat;
        switch (uvCompNum) {
        case 1:
            uvFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        case 2:
            uvFormat = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case 3:
            uvFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        default:
            assert(false);
        }
        res.stride += sizeof(float) * uvCompNum;
        res.inputs.push_back({
            "TEXCOORD",
            0,
            uvFormat,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        });
    }
    res.vertFloats.reserve(floatsNum);

    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D& vert = mesh->mVertices[i];
        addVector(vert);
        for (auto stage : pipeline) {
            stage(i);
        }
    }
    assert(res.vertFloats.size() == floatsNum);

    static constexpr int INDEX_PER_FACE = 3;
    const int indicesNum = mesh->mNumFaces * INDEX_PER_FACE;
    res.indices.reserve(indicesNum);
    for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        const uint32_t indicesNum = face.mNumIndices;
        assert(indicesNum == INDEX_PER_FACE);
        for (uint32_t j = 0; j < indicesNum; ++j) {
            res.indices.push_back(face.mIndices[j]);
        }
    }
    assert(res.indices.size() == indicesNum);

    return res;
}

void AddMesh(const aiScene* scene, int meshIdx, MeshComponent* meshComp)
{
    MeshData mesh = GatherMeshData(scene, meshIdx);
    Renderer* renderer = meshComp->GetGame()->GetRenderer();
    GeometryData::PTR geom = std::make_shared<GeometryData>(
        renderer->GetDevice(),
        static_cast<void*>(mesh.vertFloats.data()), (int)(sizeof(float) * mesh.vertFloats.size()),
        mesh.indices.data(), (int)(sizeof(uint32_t) * mesh.indices.size()),
        std::vector<uint32_t>{mesh.stride}, std::vector<uint32_t>{0});
    meshComp->SetGeometry(geom);
    meshComp->SetShader(renderer->GetUtils()->GetAdvMeshShader(renderer));
    // TODO: somehow use input layout from MeshData
}

#define NOMINMAX

#include "ShadowMapper.h"

#include "Renderer.h"
#include "DrawComponent.h"
#include "os/Window.h"

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <algorithm>
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

static constexpr float FOV = 1.57f;
static constexpr float ASPECT_RATIO = 800.0f / 800.0f;
static constexpr float NEAR_PLANE = 0.5f;
static constexpr float FAR_PLANE = 100.0f;
static constexpr float ZMULT = 10.0f;
static const Math::Vector3 LIGHT_DIR = Math::Vector3::Transform(Math::Vector3::UnitX, Math::Quaternion::CreateFromYawPitchRoll(Math::Pi / 4.0f, 0.0f, Math::Pi / 4.0f));

ID3D11Buffer* ShadowMapper::cbVSBuf{nullptr};
ID3D11Buffer* ShadowMapper::cbGSBuf{nullptr};
ID3D11Buffer* ShadowMapper::cbPSBuf{nullptr};

std::vector<Math::Vector4> GetFrustumCornersWorldSpace(const Math::Matrix& proj, const Math::Matrix& view)
{
    std::vector<Math::Vector4> frustumCorners;

    const auto inv = (view * proj).Invert();
    for (uint32_t x = 0; x < 2; ++x) {
        for (uint32_t y = 0; y < 2; ++y) {
            for (uint32_t z = 0; z < 2; ++z) {
                /*const auto test = Math::Vector4(
                    2.0f * x - 1.0f,
                    2.0f * y - 1.0f,
                    2.0f * z - 1.0f,
                    1.0f
                );*/
                const Math::Vector4 pt = Math::Vector4::Transform(
                    Math::Vector4(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        /*2.0f * z - 1.0f*/(float)z,
                        1.0f
                    ), 
                    inv);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

Math::Matrix CreatePerspective(float nearPlane, float farPlane) 
{
    return Math::Matrix::CreatePerspectiveFieldOfView(FOV, ASPECT_RATIO, nearPlane, farPlane);
}

ShadowMapper::ShadowMapper(Renderer* renderer)
{
    const wchar_t* path = L"shaders/cascade_shadows.hlsl";
    ID3DBlob* vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    auto res = D3DCompileFromFile(path,
        nullptr /*macros*/,
        nullptr /*include*/,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vertexBC,
        &errorVertexCode);

    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (errorVertexCode) {
            char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

            std::cout << compileErrors << std::endl;
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
        {
            //MessageBox(hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
        }

        //return 0;
    }

    //ID3DBlob* pixelBC;
    //ID3DBlob* errorPixelCode;
    //res = D3DCompileFromFile(path, nullptr /*macros*/, nullptr /*include*/, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);

    ID3DBlob* geomBC;
    ID3DBlob* errorGeomCode;
    res = D3DCompileFromFile(path, nullptr /*macros*/, nullptr /*include*/, "GSScene", "gs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &geomBC, &errorGeomCode);
    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (errorGeomCode) {
            char* compileErrors = (char*)(errorGeomCode->GetBufferPointer());

            std::cout << compileErrors << std::endl;
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
        {
            //MessageBox(hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
        }

        //return 0;
    }
    renderer->GetDevice()->CreateVertexShader(
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        nullptr, &vertShader);

    renderer->GetDevice()->CreateGeometryShader(
        geomBC->GetBufferPointer(),
        geomBC->GetBufferSize(),
        nullptr, &geomShader);

    /*device->CreatePixelShader(
        pixelBC->GetBufferPointer(),
        pixelBC->GetBufferSize(),
        nullptr, &pixShader);*/

    const D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        D3D11_INPUT_ELEMENT_DESC {
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        D3D11_INPUT_ELEMENT_DESC {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
    };
    renderer->GetDevice()->CreateInputLayout(
        inputElements,
        std::size(inputElements),
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        &layout);

    const D3D11_BUFFER_DESC cbVSDescs[] = {
        {
            sizeof(CBVS),			// UINT ByteWidth;
            D3D11_USAGE_DYNAMIC,		// D3D11_USAGE Usage;
            D3D11_BIND_CONSTANT_BUFFER, // UINT BindFlags;
            D3D11_CPU_ACCESS_WRITE,		// UINT CPUAccessFlags;
            0,							// UINT MiscFlags;
            0,							// UINT StructureByteStride;
        }
    };
    //assert(sizeof(MeshCBPS) % 16 == 0);
    const D3D11_BUFFER_DESC cbGSDescs[] = {
        {
            sizeof(CBGS),			// UINT ByteWidth;
            D3D11_USAGE_DYNAMIC,		// D3D11_USAGE Usage;
            D3D11_BIND_CONSTANT_BUFFER, // UINT BindFlags;
            D3D11_CPU_ACCESS_WRITE,		// UINT CPUAccessFlags;
            0,							// UINT MiscFlags;
            0,							// UINT StructureByteStride;
        }
    };

    D3D11_TEXTURE2D_DESC depthTextureDesc = {};
    ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
    depthTextureDesc.Width = 2000U;
    depthTextureDesc.Height = 2000U;
    depthTextureDesc.MipLevels = 1;
    depthTextureDesc.ArraySize = CASCADES_COUNT;
    depthTextureDesc.SampleDesc.Count = 1;
    depthTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    depthTextureDesc.SampleDesc.Count = 1;
    depthTextureDesc.SampleDesc.Quality = 0;

    ID3D11Texture2D* dsTexutre;
    if (FAILED(renderer->GetDevice()->CreateTexture2D(&depthTextureDesc, NULL, &dsTexutre))) {
        exit(-1);
        return;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvDesc.Texture2DArray.ArraySize = CASCADES_COUNT;
    //dsvDesc.Texture2DArray.FirstArraySlice = 0;

    renderer->device->CreateDepthStencilView(dsTexutre, &dsvDesc, &depthBuf);
    //dsTexutre->Release();

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};

    // Depth test parameters
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    // Stencil test parameters
    dsDesc.StencilEnable = false;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    renderer->GetDevice()->CreateDepthStencilState(&dsDesc, &depthState);

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    rtvDesc.Texture2DArray.MipSlice = 0;
    rtvDesc.Texture2DArray.ArraySize = CASCADES_COUNT;

    //renderer->GetDevice()->CreateRenderTargetView(depthBuf->GetResource(), &rtvDesc, &rtv);

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    //setup the description of the shader resource view
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
    shaderResourceViewDesc.Texture2DArray.ArraySize = 4;
    shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;

    //create the shader resource view.
    renderer->GetDevice()->CreateShaderResourceView(dsTexutre, &shaderResourceViewDesc, &depthShaderRes);

    {
        CD3D11_RASTERIZER_DESC rastDesc = {};
        rastDesc.CullMode = D3D11_CULL_FRONT;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        /*rastDesc.DepthBias = 1.f;
        rastDesc.DepthBiasClamp = 0.05f;
        rastDesc.SlopeScaledDepthBias = 0.05f;*/
        res = renderer->GetDevice()->CreateRasterizerState(&rastDesc, &cullFront);
    }
    {
        CD3D11_RASTERIZER_DESC rastDesc = {};
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        res = renderer->GetDevice()->CreateRasterizerState(&rastDesc, &cullBack);
    }

    renderer->GetDevice()->CreateBuffer(&cbVSDescs[0], nullptr, &cbVSBuf);
    renderer->GetDevice()->CreateBuffer(&cbGSDescs[0], nullptr, &cbGSBuf);
    renderer->GetDevice()->CreateBuffer(&cbGSDescs[0], nullptr, &cbPSBuf);
}

void ShadowMapper::CastShadows(Renderer* renderer)
{
    CBVS cbVS;
    CBGS cbGS;

    const float step = (FAR_PLANE - NEAR_PLANE) * (1.0f / CASCADES_COUNT);
    Math::Vector3 ldNorm = LIGHT_DIR;
    ldNorm.Normalize();
    for (int i = 0; i < CASCADES_COUNT; ++i) {
        const float nearPlane = NEAR_PLANE + i * step;
        const float farPlane = nearPlane + step;
        const Math::Matrix proj = CreatePerspective(nearPlane, farPlane);
        const auto corners = GetFrustumCornersWorldSpace(proj, renderer->GetViewMatrix());

        Math::Vector3 center = Math::Vector3::Zero;
        for (const auto& v : corners) {
            center += Math::Vector3(v);
        }
        center /= (float)corners.size();

        const auto lightView = Math::Matrix::CreateLookAt(
            center,
            center + ldNorm,
            Math::Vector3(0.0f, 0.0f, 1.0f)
        );

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();
        for (const auto& v : corners) {
            const auto trf = Math::Vector4::Transform(v, lightView);
            // my coordinates are fucked up :)
            minX = std::min(minX, -trf.y);
            maxX = std::max(maxX, -trf.y);
            minY = std::min(minY, trf.z);
            maxY = std::max(maxY, trf.z);
            minZ = std::min(minZ, trf.x);
            maxZ = std::max(maxZ, trf.x);
        }
        if (minZ < 0) {
            minZ *= ZMULT;
        } else {
            minZ /= ZMULT;
        }
        if (maxZ < 0) {
            maxZ /= ZMULT;
        } else {
            maxZ *= ZMULT;
        }

        //const Math::Matrix lightProjection = Math::Matrix::CreateOrthographic(maxX - minX, maxY - minY, minZ, maxZ);
        const Math::Matrix lightProjection = Math::Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
        const Math::Matrix lightViewProj = lightView * lightProjection;
        cbGS.lightSpaceMatrices[i] = lightViewProj.Transpose();

        /*std::cout << "{\n";
        for (const auto& v : corners) {
            const auto v3 = Math::Vector3(v);
            std::cout << '{' << v3.x << ", " << v3.y << ", " << v3.z << "},\n";
        }
        std::cout << "}\n";*/
    }

    auto context = renderer->GetDeviceContext();
    {
        D3D11_MAPPED_SUBRESOURCE subres;
        context->Map(cbVSBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
        memcpy(subres.pData, &cbVS, sizeof(cbVS));
        context->Unmap(cbVSBuf, 0);
    }
    {
        D3D11_MAPPED_SUBRESOURCE subres;
        context->Map(cbGSBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
        memcpy(subres.pData, &cbGS, sizeof(cbGS));
        context->Unmap(cbGSBuf, 0);
    }
    {
        D3D11_MAPPED_SUBRESOURCE subres;
        context->Map(cbPSBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
        memcpy(subres.pData, &cbGS, sizeof(cbGS));
        context->Unmap(cbPSBuf, 0);
    }
    context->IASetInputLayout(layout);
    context->VSSetShader(vertShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &cbVSBuf);
    context->GSSetShader(geomShader, nullptr, 0);
    context->GSSetConstantBuffers(0, 1, &cbGSBuf);
    context->PSSetShader(nullptr, nullptr, 0);

    renderer->GetDeviceContext()->ClearDepthStencilView(depthBuf, D3D11_CLEAR_DEPTH, 1.0f, 0);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(2000);
    viewport.Height = static_cast<float>(2000);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &viewport);

    // Bind target
    //ID3D11RenderTargetView* nullRenderTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
    renderer->GetDeviceContext()->OMSetRenderTargets(0, nullptr, depthBuf);

    context->RSSetState(cullFront);
    for (DrawComponent* comp : renderer->components) {
        comp->Draw(renderer, true);
    }
    context->RSSetState(cullBack);

    context->GSSetShader(nullptr, nullptr, 0);
    renderer->GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
    context->PSSetShaderResources(4, 1, &depthShaderRes);
    context->PSSetConstantBuffers(1, 1, &cbPSBuf);
}

void ShadowMapper::SetCBVS(ID3D11DeviceContext* context, int slot, void* data)
{
    D3D11_MAPPED_SUBRESOURCE subres;
    context->Map(cbVSBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &subres);
    memcpy(subres.pData, data, sizeof(CBVS));
    context->Unmap(cbVSBuf, 0);
}

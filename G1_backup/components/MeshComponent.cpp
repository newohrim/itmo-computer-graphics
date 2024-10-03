#include "MeshComponent.h"

#include "core/Game.h"
#include "render/Renderer.h"
#include "render/RenderUtils.h"
#include "render/Shader.h"
#include "render/GeometryData.h"
#include "CompositeComponent.h"
#include "materials/DefaultMeshMaterial.h"
#include "components/ThirdPersonCamera.h"

#include "render/ShadowMapper.h"

MeshComponent::MeshComponent(Game* game, Compositer* parent)
	: DrawComponent(game, parent)
	, parent(parent)
{
	auto mat = std::make_shared<DefaultMeshMaterial>(GetGame()->GetRenderer()->GetUtils()->GetAdvMeshShader(GetGame()->GetRenderer(), DefaultMeshMaterial::CBVS::GetBufferSize(), DefaultMeshMaterial::CBPS::GetBufferSize()));
	SetMaterial(mat);
}

void MeshComponent::Draw(Renderer* renderer, bool geomOnly)
{
	// TODO: remove overrided draw func
	DrawComponent::Draw(renderer, geomOnly);
}

void MeshComponent::UpdateCBVS(Renderer* renderer, ConstantBuffers::CBuf& cBuf)
{
	const Math::Matrix worldTransform = parent->GetWorldTransform().Transpose();
	CB_SET_FIELD(cBuf, worldTransform);
	const Math::Matrix& viewProj = renderer->GetViewProjMatrix();
	CB_SET_FIELD(cBuf, viewProj);

	DrawComponent::UpdateCBVS(renderer, cBuf);
}

void MeshComponent::UpdateCBPS(Renderer* renderer, ConstantBuffers::CBuf& cBuf)
{
	const float step = (100.0f - 0.5f) * (1.0f / DefaultMeshMaterial::NR_CASCADES);
	float cascadePlaneDistances[4];
	for (int i = 0; i < DefaultMeshMaterial::NR_CASCADES; ++i) {
		const float nearPlane = 0.5f + i * step;
		const float farPlane = nearPlane + step;
		cascadePlaneDistances[i] = farPlane;
	}
	CB_SET_FIELD(cBuf, cascadePlaneDistances, std::size(cascadePlaneDistances));
	Math::Matrix viewMatr = renderer->GetViewMatrix().Transpose();
	CB_SET_FIELD(cBuf, viewMatr);
	CB_SET_FIELD(cBuf, color);
	const Math::Color uAmbientLight = Math::Color{0.2f, 0.2f, 0.2f}; // should be in lights/renderer
	CB_SET_FIELD(cBuf, uAmbientLight);
	const ThirdPersonCamera* cam = static_cast<ThirdPersonCamera*>(GetGame()->GetActiveCamera());
	const Math::Vector4 uCameraPos = Math::Vector4(cam->GetCameraPos());
	CB_SET_FIELD(cBuf, uCameraPos);
	const float uSpecPower = 0.0f; // should be in material
	CB_SET_FIELD(cBuf, uSpecPower);
	//renderer->PopulateLightsBuffer(cbPS); // TODO: TEMP E2
	const int isTextureSet = tex.IsValid() ? 1 : 0;
	CB_SET_FIELD(cBuf, isTextureSet);

	DrawComponent::UpdateCBPS(renderer, cBuf);
}

MeshComponent* MeshComponent::Build(Mesh::PTR mesh, CompositeComponent* parent)
{
	if (!mesh || mesh->GetRoot().geoms.empty()) {
		return nullptr;
	}
	return BuildMeshNode(mesh->GetRoot(), parent);
}

MeshComponent* MeshComponent::BuildMeshNode(const Mesh::MeshNode& node, CompositeComponent* parent)
{
	parent->SetPosition(node.pos);
	parent->SetRotation(node.rot);
	parent->SetScale(node.scale);
	Renderer* renderer = parent->GetGame()->GetRenderer();
	MeshComponent* tempRes = nullptr;
	for (const GeometryData::PTR& geom : node.geoms) {
		MeshComponent* meshComp = new MeshComponent(parent->GetGame(), parent);
		meshComp->SetGeometry(geom);
		//auto mat = std::make_shared<DefaultMeshMaterial>(parent->GetGame()->GetRenderer()->GetUtils()->GetAdvMeshShader(parent->GetGame()->GetRenderer(), sizeof(DefaultMeshMaterial::CBVS), sizeof(DefaultMeshMaterial::CBPS)));
		//meshComp->SetMaterial(mat);
		if (!tempRes) {
			tempRes = meshComp;
		}
	}
	for (const Mesh::MeshNode& child : node.children) {
		CompositeComponent* childComp = new CompositeComponent(parent->GetGame(), parent);
		BuildMeshNode(child, childComp);
	}

	return tempRes;
}

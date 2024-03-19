#include "MeshComponent.h"

#include "core/Game.h"
#include "render/Renderer.h"
#include "render/RenderUtils.h"
#include "render/Shader.h"
#include "render/GeometryData.h"
#include "CompositeComponent.h"

MeshComponent::MeshComponent(Game* game, Compositer* parent)
	: DrawComponent(game, parent)
	, parent(parent)
{
}

void MeshComponent::Draw(Renderer* renderer)
{
	auto shader = GetShader();
	auto context = renderer->GetDeviceContext();
	auto window = renderer->GetWindow();
	auto cbVS = RenderUtils::MeshCBVS{};
	cbVS.worldTransform = parent->GetWorldTransform().Transpose();
	cbVS.viewProj = renderer->GetViewMatrix();
	auto cbPS = RenderUtils::QuadCBPS{};
	cbPS.color = color;
	shader.lock()->SetCBVS(context, 0, &cbVS);
	shader.lock()->SetCBPS(context, 0, &cbPS);
	if (tex.IsValid()) {
		tex.Activate(context);
	}
	shader.lock()->Activate(context);
	DrawComponent::Draw(renderer);
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
		meshComp->SetShader(renderer->GetUtils()->GetAdvMeshShader(renderer));
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

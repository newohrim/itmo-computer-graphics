#include "MeshComponent.h"

#include "render/Renderer.h"
#include "render/Shader.h"
#include "core/Compositer.h"

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
	DrawComponent::Draw(renderer);
}

#include "QuadComponent.h"

#include "core/Game.h"
#include "render/GeometryData.h"
#include "render/Shader.h"
#include "render/Renderer.h"
#include "render/RenderUtils.h"

QuadComponent::QuadComponent(Game* game) 
	: DrawComponent(game)
{
	Renderer* renderer = GetGame()->GetRenderer();
	GeometryData::PTR geom = renderer->GetUtils()->GetQuadGeom(renderer);
	SetGeometry(geom);
	Shader::PTR shader = renderer->GetUtils()->GetQuadShader(renderer);
	SetShader(shader);
}

void QuadComponent::Initialize()
{
	DrawComponent::Initialize();
}

void QuadComponent::Draw(Renderer* renderer)
{
	// TODO: set color
	// TODO: set size
	auto shader = GetShader();
	auto context = renderer->GetDeviceContext();
	auto cbVS = RenderUtils::QuadCBVS{};
	cbVS.offset = DirectX::SimpleMath::Vector4{offset.x, offset.y, 0, 0};
	cbVS.size = DirectX::SimpleMath::Vector4{size.x, size.y, 0, 0};
	auto cbPS = RenderUtils::QuadCBPS{};
	cbPS.color = color;
	shader.lock()->SetCBVS(context, 0, &cbVS);
	shader.lock()->SetCBPS(context, 0, &cbPS);
	DrawComponent::Draw(renderer);
}

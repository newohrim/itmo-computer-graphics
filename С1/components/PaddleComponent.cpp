#include "PaddleComponent.h"

#include "core/Math.h"
#include "core/Compositer.h"
#include "core/Game.h"
#include "os/Window.h"
#include "input/InputDevice.h"
#include "components/MoveComponent.h"
#include "components/QuadComponent.h"
#include "render/Renderer.h"
#include <algorithm>

#include <cassert>

PaddleComponent::PaddleComponent(Game* game, Math::Vector2 _paddleQuad, bool _isPlayerOne)
	: CompositeComponent(game, nullptr),
	  paddleQuad(_paddleQuad),
	  isPlayerOne(_isPlayerOne)
{
}

void PaddleComponent::Initialize(Compositer* parent)
{
	Game* game = GetGame();
	QuadComponent* quad = new QuadComponent(game, this);
	Renderer* renderer = game->GetRenderer();
	quad->SetSize(paddleQuad);
	quad->SetGeometry(renderer->GetUtils()->GetQuadGeom(renderer));
	quad->SetShader(renderer->GetUtils()->GetQuadShader(renderer));
	moveComp = new MoveComponent(game, this);
	moveComp->SetMoveSpeed(300.0f);
	CompositeComponent::Initialize(parent);
}

void PaddleComponent::ProceedInput(InputDevice* inpDevice)
{
	Math::Vector2 dir = Math::Vector2::Zero;
	if (inpDevice->IsKeyDown(isPlayerOne ? Keys::W : Keys::Up)) {
		dir = Math::Vector2::UnitY;
	}
	if (inpDevice->IsKeyDown(isPlayerOne ? Keys::S : Keys::Down)) {
		dir = -Math::Vector2::UnitY;
	}
	moveComp->Move(dir);
}

void PaddleComponent::Update(float deltaTime, Compositer* parent)
{
	CompositeComponent::Update(deltaTime, parent);
	const Window* window = GetGame()->GetWindow();
	Math::Vector2 pos = GetPosition();
	pos.y = max(30.0f, pos.y);
	pos.y = min(window->GetHeigth() - 30.0f, pos.y);
	SetPosition(pos);
}

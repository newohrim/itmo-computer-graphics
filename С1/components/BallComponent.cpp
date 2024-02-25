#include "BallComponent.h"

#include "core/Compositer.h"
#include "components/MoveComponent.h"
#include "components/PaddleComponent.h"
#include "components/QuadComponent.h"
#include "core/Game.h"
#include "os/Window.h"
#include "ScoreBoard.h"

BallComponent::BallComponent(Game* game, PaddleComponent* _paddleCompA, PaddleComponent* _paddleCompB, Math::Vector2 _ballSize)
	: CompositeComponent(game, nullptr),
	  paddleCompA(_paddleCompA),
	  paddleCompB(_paddleCompB),
	  ballSize(_ballSize)
{
}

void BallComponent::Initialize(Compositer* parent)
{
	Game* game = GetGame();
	quadComp = new QuadComponent(game, this);
	quadComp->SetSize(ballSize);
	moveComp = new MoveComponent(game, this);
	moveComp->SetMoveSpeed(300.0f);
	moveDir = Math::Vector2{-3.0f, -1.0f};
	moveDir.Normalize();
	CompositeComponent::Initialize(parent);
}

void BallComponent::Update(float deltaTime, Compositer* parent)
{
	CompositeComponent::Update(deltaTime, parent);
	Math::Vector2 pos = GetPosition();
	Window* window = GetGame()->GetWindow();
	if (pos.x < 0.0f) {
		ScoreBoard::AddScoreA(1, window);
		GetGame()->Restart();
		return;
	}
	if (pos.x > window->GetWidth()) {
		ScoreBoard::AddScoreB(1, window);
		GetGame()->Restart();
		return;
	}
	const Math::Vector2 ballHaldSize = ballSize / 2.0f;
	const Math::Vector2 paddlePosA = paddleCompA->GetPosition();
	const Math::Vector2 paddlePosB = paddleCompB->GetPosition();
	if (pos.x > paddlePosA.x && pos.x < paddlePosB.x) {
		const Math::Vector2 paddleHalfSizeA = paddleCompA->GetPaddleSize() / 2.0f;
		if (pos.x - ballHaldSize.x < paddlePosA.x + paddleHalfSizeA.x) {
			if (pos.y - ballHaldSize.y < paddlePosA.y + paddleHalfSizeA.y &&
				pos.y + ballHaldSize.y > paddlePosA.y - paddleHalfSizeA.y) {
				HandlePaddleCollision();
			}
		}
		const Math::Vector2 paddleHalfSizeB = paddleCompB->GetPaddleSize() / 2.0f;
		if (pos.x + ballHaldSize.x > paddlePosB.x - paddleHalfSizeB.x) {
			if (pos.y - ballHaldSize.y < paddlePosB.y + paddleHalfSizeB.y &&
				pos.y + ballHaldSize.y > paddlePosB.y - paddleHalfSizeB.y) {
				HandlePaddleCollision();
			}
		}
	}
	if (pos.y + ballHaldSize.y > window->GetHeigth() ||
		pos.y - ballHaldSize.y < 0.0f) {
		moveDir.y *= -1.0f;
	}
	moveComp->Move(moveDir);
}

void BallComponent::HandlePaddleCollision()
{
	moveDir.x *= -1.0f;
	moveComp->SetMoveSpeed(moveComp->GetMoveSpeed() * 1.1f);
	ballSize -= Math::Vector2{1.0f};
	if (ballSize.LengthSquared() < 4.0f) {
		ballSize = Math::Vector2{4.0f};
	}
	quadComp->SetSize(ballSize);
}

#pragma once

#include "components/CompositeComponent.h"
#include "core/Math.h"

class MoveComponent;
class PaddleComponent;
class QuadComponent;

class BallComponent : public CompositeComponent {
public:
	BallComponent(Game* game, PaddleComponent* paddleCompA, PaddleComponent* paddleCompB, Math::Vector2 ballSize);

	void Initialize(Compositer* parent = nullptr) override;

	void Update(float deltaTime, Compositer* parent = nullptr) override;

private:
	void HandlePaddleCollision();

private:
	MoveComponent* moveComp;
	QuadComponent* quadComp;
	PaddleComponent* paddleCompA;
	PaddleComponent* paddleCompB;
	
	Math::Vector2 moveDir = Math::Vector2::Zero;
	Math::Vector2 ballSize;
};

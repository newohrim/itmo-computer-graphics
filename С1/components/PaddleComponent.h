#pragma once

#include "components/CompositeComponent.h"

class MoveComponent;

class PaddleComponent : public CompositeComponent {
public:
	PaddleComponent(Game* game, Math::Vector2 paddleQuad, bool isPlayerOne);

	void Initialize(Compositer* parent = nullptr) override;

	void ProceedInput(InputDevice* inpDevice) override;

	void Update(float deltaTime, Compositer* parent = nullptr) override;

	Math::Vector2 GetPaddleSize() const { return paddleQuad; }

private:
	MoveComponent* moveComp;
	Math::Vector2 paddleQuad;
	bool isPlayerOne;
};


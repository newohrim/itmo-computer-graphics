#pragma once

#include "core/Component.h"
#include "core/Math.h"

class MoveComponent : public Component {
public:
	MoveComponent(Game* game, Compositer* compositer = nullptr);

	void Move(Math::Vector2 dir);

	void Update(float deltaTime, Compositer* parent = nullptr) override;

	float GetMoveSpeed() const { return moveSpeed; }
	void SetMoveSpeed(float _moveSpeed) { moveSpeed = _moveSpeed; }

protected:
	Math::Vector2 direction = Math::Vector2::Zero;
	float moveSpeed = 100.0f;
};

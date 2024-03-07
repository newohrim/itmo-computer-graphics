#include "MoveComponent.h"

#include "core/Compositer.h"

MoveComponent::MoveComponent(Game* game, Compositer* compositer)
	: Component(game, compositer)
{
}

void MoveComponent::Move(Math::Vector2 dir)
{
	direction = dir;
}

void MoveComponent::Update(float deltaTime, Compositer* parent)
{
	if (!parent || direction.LengthSquared() < 0.001f) {
		return;
	}
	const Math::Vector2 pos = parent->GetPosition() + direction * moveSpeed * deltaTime;
	parent->SetPosition(Math::Vector3{pos.x, pos.y, 0.0f});
	direction = Math::Vector2::Zero;
}

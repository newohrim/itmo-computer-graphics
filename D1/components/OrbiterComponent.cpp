#include "OrbiterComponent.h"

#include "core/Compositer.h"
#include <cmath>
#include <iostream>

OrbiterComponent::OrbiterComponent(Game* game, Compositer* parent, Math::Vector3 _center, float _radius, float _orbitDir)
	: MoveComponent(game, parent)
	, center(_center)
	, orbitDir(_orbitDir)
	, radius(_radius)
{
	angle = ((float)rand() / RAND_MAX) * (2 * Math::Pi);
}

void OrbiterComponent::Update(float deltaTime, Compositer* parent)
{
	{
		angle += moveSpeed * deltaTime;
		Math::Vector3 pos;
		pos.x = center.x + cos(angle) * radius;
		pos.y = center.y + sin(angle) * radius;
		parent->SetPosition(pos);
	}
	{
		const Math::Quaternion rot = parent->GetRotation();
		const Math::Quaternion inc = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::UnitZ, 1.0f * deltaTime);
		parent->SetRotation(Math::Quaternion::Concatenate(rot, inc));
	}
}

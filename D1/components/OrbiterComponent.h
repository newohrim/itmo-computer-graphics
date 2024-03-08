#pragma once

#include "components/MoveComponent.h"

class OrbiterComponent : public MoveComponent {
public:
	OrbiterComponent(Game* game, Compositer* parent, Math::Vector3 center, float radius, float orbitDir = 1.0f);

	void Update(float deltaTime, Compositer* parent = nullptr) override;

protected:
	Math::Vector3 center;
	float orbitDir;
	float angle = 0.0f;
	float radius;
};

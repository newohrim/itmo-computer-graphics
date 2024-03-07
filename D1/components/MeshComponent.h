#pragma once

#include "render/DrawComponent.h"
#include "core/Math.h"

class MeshComponent : public DrawComponent {
public:
	MeshComponent(Game* game, Compositer* parent);

	void Draw(Renderer* renderer) override;

protected:
	Compositer* parent;
	Math::Color color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

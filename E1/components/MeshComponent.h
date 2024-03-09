#pragma once

#include "render/DrawComponent.h"
#include "core/Math.h"

class MeshComponent : public DrawComponent {
public:
	MeshComponent(Game* game, Compositer* parent);

	void Draw(Renderer* renderer) override;

	void SetColor(const Math::Color& _color) { color = _color; }

protected:
	Compositer* parent;
	Math::Color color{ 1.0f, 1.0f, 1.0f, 1.0f };
};

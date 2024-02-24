#pragma once

#include "render/DrawComponent.h"

class QuadComponent : public DrawComponent {
public:
	using Color = DirectX::SimpleMath::Color;
	using Vector2 = DirectX::SimpleMath::Vector2;

	QuadComponent(Game* game);

	void Initialize(Compositer* parent = nullptr) override { parentRef = parent; }
	void Draw(Renderer* renderer) override;

	void SetColor(const Color& _color) { color = _color; }
	void SetSize(Vector2 _size) { size = _size; }

private:
	Compositer* parentRef = nullptr;

	Color color{1.0f, 1.0f, 1.0f, 1.0f};
	Vector2 size{10.0f, 60.0f};
};

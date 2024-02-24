#pragma once

#include "render/DrawComponent.h"

class QuadComponent : public DrawComponent {
public:
	using Color = DirectX::SimpleMath::Color;
	using Vector2 = DirectX::SimpleMath::Vector2;

	QuadComponent(Game* game);

	void Initialize() override;
	void Draw(Renderer* renderer) override;

	void SetColor(const Color& _color) { color = _color; }
	void SetSize(Vector2 _size) { size = _size; }

private:
	Color color{1.0f, 1.0f, 1.0f, 1.0f};
	Vector2 offset{0.0f, 0.0f};
	Vector2 size{1.0f, 1.0f};
};

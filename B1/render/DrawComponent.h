#pragma once

#include "core/Component.h"
#include <cstdint>
#include <memory>

class Renderer;

struct Shader;
struct GeometryData;

class DrawComponent : public Component
{
public:
	DrawComponent(Game* game);
	~DrawComponent();

	void Initialize() override;
	void Draw(Renderer* renderer);

	void SetShader(const std::shared_ptr<Shader> shader) { linkedShader = shader; }
	void SetGeometry(const std::shared_ptr<GeometryData> _geom) { geom = _geom; }

private:
	std::shared_ptr<Shader> linkedShader;
	std::shared_ptr<GeometryData> geom;
};

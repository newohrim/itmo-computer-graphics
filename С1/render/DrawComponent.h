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
	DrawComponent(Game* game, Compositer* compositer = nullptr);
	~DrawComponent();

	void Initialize(Compositer* parent = nullptr) override;
	virtual void Draw(Renderer* renderer);

	std::weak_ptr<Shader> GetShader() const { return linkedShader; }
	void SetShader(const std::shared_ptr<Shader> shader) { linkedShader = shader; }
	void SetGeometry(const std::shared_ptr<GeometryData> _geom) { geom = _geom; }

private:
	std::shared_ptr<Shader> linkedShader;
	std::shared_ptr<GeometryData> geom;
};

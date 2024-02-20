#pragma once

#include <memory>

class Renderer;
struct GeometryData;
struct Shader;

class RenderUtils {
public:
	~RenderUtils();

	std::shared_ptr<Shader> GetQuadShader(Renderer* renderer);
	std::shared_ptr<GeometryData> GetQuadGeom(Renderer* renderer);
	std::shared_ptr<GeometryData> GetTriGeom(Renderer* renderer, const void* verts, int vertsSize);

private:
	std::shared_ptr<Shader> CreateQuadShader(Renderer* renderer);
	std::shared_ptr<GeometryData> CreateQuadGeom(Renderer* renderer);

private:
	std::shared_ptr<Shader> quadShader;
	std::shared_ptr<GeometryData> quadGeom;
};

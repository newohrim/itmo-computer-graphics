#pragma once

#include <memory>
#include <SimpleMath.h>

class Renderer;
struct GeometryData;
struct Shader;

class RenderUtils {
public:
	using Color = DirectX::SimpleMath::Color;
	using Vector4 = DirectX::SimpleMath::Vector4;

	struct QuadCBVS {
		Vector4 offset;
		Vector4 size;
	};

	struct QuadCBPS {
		Color color;
	};

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

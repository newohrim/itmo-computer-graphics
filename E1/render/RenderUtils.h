#pragma once

#include <memory>
#include "core/Math.h"

class Renderer;
struct GeometryData;
struct Shader;

class RenderUtils {
public:
	struct QuadCBVS {
		Math::Vector4 offset;
		Math::Vector4 size;
	};

	struct QuadCBPS {
		Math::Color color;
	};

	struct MeshCBVS {
		Math::Matrix worldTransform;
		Math::Matrix viewProj;
	};

	using MeshCBPS = QuadCBPS;

	~RenderUtils();

	std::shared_ptr<Shader> GetQuadShader(Renderer* renderer);
	std::shared_ptr<GeometryData> GetQuadGeom(Renderer* renderer);
	std::shared_ptr<GeometryData> GetTriGeom(Renderer* renderer, const void* vertFloats, int vertsSize);

	std::shared_ptr<Shader> GetMeshShader(Renderer* renderer);
	std::shared_ptr<Shader> GetAdvMeshShader(Renderer* renderer);
	std::shared_ptr<GeometryData> GetCubeGeom(Renderer* renderer);
	std::shared_ptr<GeometryData> GetSphereGeom(Renderer* renderer);

private:
	std::shared_ptr<Shader> CreateQuadShader(Renderer* renderer);
	std::shared_ptr<GeometryData> CreateQuadGeom(Renderer* renderer);

	std::shared_ptr<Shader> CreateMeshShader(Renderer* renderer);
	std::shared_ptr<Shader> CreateAdvMeshShader(Renderer* renderer);
	std::shared_ptr<GeometryData> CreateCubeGeom(Renderer* renderer);
	std::shared_ptr<GeometryData> CreateSphereGeom(Renderer* renderer);

private:
	std::shared_ptr<Shader> quadShader;
	std::shared_ptr<GeometryData> quadGeom;

	std::shared_ptr<Shader> meshShader;
	std::shared_ptr<Shader> meshAdvShader;
	std::shared_ptr<GeometryData> cubeGeom;
	std::shared_ptr<GeometryData> sphereGeom;
};

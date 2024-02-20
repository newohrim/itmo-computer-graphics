#include "RenderUtils.h"

#include "Renderer.h"
#include "GeometryData.h"
#include "Shader.h"

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <d3dcommon.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

RenderUtils::~RenderUtils()
{
	// TODO: terminate all geom data
}

std::shared_ptr<Shader> RenderUtils::GetQuadShader(Renderer* renderer)
{
	if (quadShader) {
		return quadShader;
	}

	quadShader = CreateQuadShader(renderer);
	return quadShader;
}

GeometryData::PTR RenderUtils::GetQuadGeom(Renderer* renderer)
{
    if (quadGeom) {
        return quadGeom;
    }
	
	quadGeom = CreateQuadGeom(renderer);
	return quadGeom;
}

std::shared_ptr<GeometryData> RenderUtils::GetTriGeom(Renderer* renderer, const void* verts, int vertsSize)
{
	const uint32_t indices[] = { 0,1,2 };
	return std::make_shared<GeometryData>(
		renderer->GetDevice(),
		verts, vertsSize,
		indices, (int)(sizeof(uint32_t) * std::size(indices)),
		std::vector<uint32_t>{ 32 }, std::vector<uint32_t>{0});
}

std::shared_ptr<Shader> RenderUtils::CreateQuadShader(Renderer* renderer)
{
	const D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};
	const D3D_SHADER_MACRO macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
	return std::make_shared<Shader>(L"shaders/MyVeryFirstShader.hlsl", renderer->GetDevice(), inputElements, 2, macros, 3);
}

GeometryData::PTR RenderUtils::CreateQuadGeom(Renderer* renderer)
{
	const DirectX::XMFLOAT4 points[8] = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	 DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	 DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	 DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	const uint32_t indices[] = { 0,1,2, 1,0,3 };
	return std::make_shared<GeometryData>(
		renderer->GetDevice(),
		points, (int)(sizeof(DirectX::XMFLOAT4) * std::size(points)),
		indices, (int)(sizeof(uint32_t) * std::size(indices)),
		std::vector<uint32_t>{ 32 }, std::vector<uint32_t>{0});
}

#pragma once

#include <vector>
#include <memory>
#include "core/Math.h"
#include "ConstantBuffers.h"

class Renderer;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

class GBuffer {
public:
	GBuffer(Renderer* renderer);
	~GBuffer();

	void Clear(Renderer* renderer);

public:
	enum VIEW_IDX {
		WORLD_POS = 0,
		NORMAL,
		ALBEDO_SPEC,
		DEPTH_STENCIL,

		_VIEW_IDX_NUM
	};
	std::vector<ID3D11RenderTargetView*> rtViews;
	std::vector<ID3D11ShaderResourceView*> srViews;
	struct ID3D11DepthStencilView* dsv;
};

class DeferredRenderer {
public:
	struct GeometryPass {
		CB_DECLARE_BEGIN(CBVS)
			CB_DECLARE_FIELD(Math::Matrix, worldTransform)
			CB_DECLARE_FIELD(Math::Matrix, viewProj)
		CB_DECLARE_END()

		CB_DECLARE_BEGIN(CBPS)
			CB_DECLARE_FIELD(Math::Color, color)
			CB_DECLARE_FIELD(Math::Vector4, uCameraPos)
			CB_DECLARE_FIELD(float, uSpecPower)
			CB_DECLARE_FIELD(int, isTextureSet)
		CB_DECLARE_END()
	};

	DeferredRenderer(Renderer* renderer);

	void Draw(Renderer* renderer);

protected:
	void DrawGeometry(Renderer* renderer);

	GBuffer gBuffer;
	struct ID3D11RasterizerState* rastState;
	struct ID3D11SamplerState* samplerState;
	struct ID3D11DepthStencilState* dsState;
	std::shared_ptr<struct Shader> geomShader;
};

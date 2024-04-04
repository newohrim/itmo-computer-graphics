#pragma once

#include <vector>
#include "core/Math.h"

class Renderer;
struct ID3D11Buffer;

class ShadowMapper
{
public:
	ShadowMapper(Renderer* renderer);

	void CastShadows(Renderer* renderer);

	static constexpr int CASCADES_COUNT = 4;

protected:
	struct CBVS {
		Math::Matrix worldTransform;
		Math::Matrix viewProj;
	};
	struct CBGS {
		Math::Matrix lightSpaceMatrices[CASCADES_COUNT];
	};

	struct ID3D11InputLayout* layout;
	struct ID3D11VertexShader* vertShader;
	struct ID3D11GeometryShader* geomShader;
	struct ID3D11DepthStencilView* depthBuf;
	struct ID3D11RenderTargetView* rtv;
	struct ID3D11DepthStencilState* depthState;
	struct ID3D11ShaderResourceView* depthShaderRes;

public:
	static void SetCBVS(struct ID3D11DeviceContext* context, int slot, void* data);

	static ID3D11Buffer* cbVSBuf;
	static ID3D11Buffer* cbGSBuf;
	static ID3D11Buffer* cbPSBuf;
};

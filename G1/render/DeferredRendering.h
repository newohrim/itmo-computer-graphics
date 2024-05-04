#pragma once

#include <vector>
#include <memory>
#include "core/Math.h"

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
		struct CBVS {
			Math::Matrix worldTransform;
			Math::Matrix viewProj;
		};
		struct CBPS {
			Math::Color color;
			Math::Vector4 uCameraPos;
			float uSpecPower = 0.25f;
			int isTextureSet = 0;
			Math::Vector2 _dummy;
		};
	};
	struct LightingPass {
		struct CBPS {
			static constexpr int NR_POINT_LIGHTS = 4;

			struct DirectionalLight {
				Math::Vector4 mDirection;
				Math::Vector4 mDiffuseColor;
				Math::Vector4 mSpecColor;
			} dirLight;
			struct PointLight {
				Math::Vector4 position;
				Math::Vector4 diffuse;
				Math::Vector4 specular;
				float constant = 0.0f;
				float linear = 0.0f;
				float quadratic = 0.0f;
				float _dummy = 0.0f;
			};
			PointLight pointLights[NR_POINT_LIGHTS];

			Math::Matrix viewMatr;
			Math::Vector4 uCameraPos;
			Math::Vector4 uAmbientLight;
			//float cascadePlaneDistances[NR_CASCADES]; doesn't work for whatever reason
			float cascadePlaneDistances[4];
			float uShininess = 32.0f; // TODO: remove from lighting
			int spotLightsNum = 0;
			Math::Vector2 _dummy;
		};
	};

	DeferredRenderer(Renderer* renderer, ID3D11RenderTargetView* targetRtv);

	void Draw(Renderer* renderer);

protected:
	void DrawGeometry(Renderer* renderer);
	void DrawLighting(Renderer* renderer);

	GBuffer gBuffer;
	ID3D11RenderTargetView* targetRtv;
	struct ID3D11RasterizerState* rastState;
	struct ID3D11SamplerState* samplerState;
	struct ID3D11DepthStencilState* dsState;
	std::shared_ptr<struct Shader> geomShader;
	std::shared_ptr<struct Shader> lightShader;
};

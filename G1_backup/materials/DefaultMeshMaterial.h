#pragma once

#include "render/Material.h"
#include "render/ConstantBuffers.h"
#include "core/Math.h"

class DefaultMeshMaterial : public Material {
public:
	static constexpr int NR_POINT_LIGHTS = 4;
	static constexpr int NR_CASCADES = 4;

	struct DirectionalLight {
		Math::Vector4 mDirection;
		Math::Color mDiffuseColor;
		Math::Color mSpecColor;
	};
	struct PointLight {
		Math::Vector4 position;
		Math::Vector4 diffuse;
		Math::Vector4 specular;
		float constant = 0.0f;
		float linear = 0.0f;
		float quadratic = 0.0f;
		float _dummy = 0.0f;
	};

	CB_DECLARE_BEGIN(CBVS)
		CB_DECLARE_FIELD(Math::Matrix, worldTransform)
		CB_DECLARE_FIELD(Math::Matrix, viewProj)
	CB_DECLARE_END()

	CB_DECLARE_BEGIN(CBPS)
		CB_DECLARE_FIELD(DirectionalLight, dirLight)
		CB_DECLARE_FIELD(PointLight[NR_POINT_LIGHTS], pointLights)
		CB_DECLARE_FIELD(Math::Matrix, viewMatr)
		CB_DECLARE_FIELD(Math::Color, color)
		CB_DECLARE_FIELD(Math::Vector4, uCameraPos)
		CB_DECLARE_FIELD(Math::Color, uAmbientLight)
		CB_DECLARE_FIELD(float[NR_CASCADES], cascadePlaneDistances)
		CB_DECLARE_FIELD(float, uSpecPower)
		CB_DECLARE_FIELD(float, uShininess)
		CB_DECLARE_FIELD(int, spotLightsNum)
		CB_DECLARE_FIELD(int, isTextureSet)
	CB_DECLARE_END()

	DefaultMeshMaterial(std::shared_ptr<Shader> shader);

	void Use(Renderer* renderer) override;

	int GetCBVSSize() const override { return sizeof(CBVS); }
	int GetCBPSSize() const override { return sizeof(CBPS); }
};

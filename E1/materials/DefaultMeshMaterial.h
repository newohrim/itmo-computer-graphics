#pragma once

#include "render/Material.h"
#include "core/Math.h"

class DefaultMeshMaterial : public Material {
public:
	struct CBVS {
		Math::Matrix worldTransform;
		Math::Matrix viewProj;
	};
	struct CBPS {
		struct DirectionalLight {
			Math::Vector4 mDirection;
			Math::Color mDiffuseColor;
			Math::Color mSpecColor;
		} dirLight;
		Math::Color color;
		Math::Vector4 uCameraPos;
		Math::Color uAmbientLight;
		float uSpecPower = 0.25f;
		Math::Vector3 _dummy1;
		bool isTextureSet = false;
		Math::Vector3 _dummy2;
	};

	DefaultMeshMaterial(std::shared_ptr<Shader> shader);

	void Use(Renderer* renderer) override;

	int GetCBVSSize() const override { return sizeof(CBVS); }
	int GetCBPSSize() const override { return sizeof(CBPS); }
};

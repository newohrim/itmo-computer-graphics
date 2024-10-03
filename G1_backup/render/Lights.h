#pragma once

#include "core/Component.h"
#include "core/Math.h"
#include "ConstantBuffers.h"


class Renderer;


class Light {
public:
	Light(Renderer* renderer, Compositer* parent);
	virtual ~Light();

	Math::Vector3 GetPos() const;

	void SetColor(const Math::Color& col) { color = col; }

	virtual void UpdateBuffer(ConstantBuffers::CBuf& buffer) const = 0;

protected:
	Renderer* GetRenderer() const;

protected:
	Compositer* parentRef;
	Math::Color color = {1.0f, 1.0f, 1.0f};
	Math::Color specularColor = {1.0f, 1.0f, 1.0f};
};


class DirectionalLight : public Light {
public:
	DirectionalLight(Renderer* renderer, Compositer* parent);

	void UpdateBuffer(ConstantBuffers::CBuf& buffer) const override;
};


class PointLight : public Light {
public:
	enum AttenuationType {
		Constant = 0,
		Linear,
		Quadratic
	};

	PointLight(Renderer* renderer, Compositer* parent);

	void UpdateBuffer(ConstantBuffers::CBuf& buffer) const override;

	void SetIntensity(float _intensity) { intensity = _intensity; }

protected:
	AttenuationType attenuation = Quadratic;
	float intensity = 10.0f;
};

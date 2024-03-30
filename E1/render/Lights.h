#pragma once

#include "core/Component.h"
#include "core/Math.h"

#include "materials/DefaultMeshMaterial.h" // TODO: TEMP E2

class Renderer;


class Light {
public:
	Light(Renderer* renderer, Compositer* parent);
	virtual ~Light();

	Math::Vector3 GetPos() const;

	void SetColor(const Math::Color& col) { color = col; }

	virtual void UpdateBuffer(DefaultMeshMaterial::CBPS& buffer) const = 0;

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

	void UpdateBuffer(DefaultMeshMaterial::CBPS& buffer) const override;
};

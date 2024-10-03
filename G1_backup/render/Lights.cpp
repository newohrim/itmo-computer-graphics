#include "Lights.h"

#include "Renderer.h"
#include "core/Game.h"
#include "core/Component.h"
#include "core/Compositer.h"

Light::Light(Renderer* renderer, Compositer* parent)
	: parentRef(parent)
{
	renderer->AddLight(this);
}

Light::~Light() 
{
	GetRenderer()->RemoveLight(this);
}

Math::Vector3 Light::GetPos() const
{
	return parentRef->GetPosition();
}

Renderer* Light::GetRenderer() const
{
	return parentRef->GetGamePtr()->GetRenderer();
}

DirectionalLight::DirectionalLight(Renderer* renderer, Compositer* parent)
	: Light(renderer, parent)
{
}

void DirectionalLight::UpdateBuffer(ConstantBuffers::CBuf& buffer) const
{
	DefaultMeshMaterial::DirectionalLight dirLight;
	dirLight.mDiffuseColor = color;
	dirLight.mSpecColor = specularColor;
	dirLight.mDirection = Math::Vector4(parentRef->GetForward());
	CB_SET_FIELD(buffer, dirLight);
}

PointLight::PointLight(Renderer* renderer, Compositer* parent)
	: Light(renderer, parent)
{
}

void PointLight::UpdateBuffer(ConstantBuffers::CBuf& buffer) const
{
	int spotLightsNum;
	if (!CB_GET_FIELD(buffer, spotLightsNum)) {
		return;
	}
	if (spotLightsNum >= DefaultMeshMaterial::NR_POINT_LIGHTS) {
		return;
	}

	DefaultMeshMaterial::PointLight* pointLights;
	if (!CB_GET_PTR(buffer, pointLights)) {
		return;
	}
	DefaultMeshMaterial::PointLight& pointL = pointLights[spotLightsNum];
	pointL.diffuse = color * intensity; // wtf?
	pointL.specular = specularColor;
	pointL.position = Math::Vector4(parentRef->GetPosition());
	switch (attenuation) {
	case Constant:
		pointL.constant = 1.0f;
		break;
	case Linear:
		pointL.linear = 1.0f;
		break;
	case Quadratic:
		pointL.quadratic = 1.0f;
		break;
	default:
		assert(false);
	}
	spotLightsNum++;
	CB_SET_FIELD(buffer, spotLightsNum);
}

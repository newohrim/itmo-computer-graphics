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

void DirectionalLight::UpdateBuffer(DefaultMeshMaterial::CBPS& buffer) const
{
	buffer.dirLight.mDiffuseColor = color;
	buffer.dirLight.mSpecColor = specularColor;
	buffer.dirLight.mDirection = Math::Vector4(parentRef->GetForward());
}

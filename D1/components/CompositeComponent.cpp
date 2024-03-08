#include "CompositeComponent.h"

#include <algorithm>

CompositeComponent::CompositeComponent(Game* game, Compositer* compositer)
	: Component(game, compositer)
{
}

CompositeComponent::~CompositeComponent()
{
	for (Component* child : children) {
		delete child;
	}
	children.clear();
}

void CompositeComponent::RemoveChild(Component* comp)
{
	// TODO: allow removing child in update by add pending list
	auto iter = std::find(children.begin(), children.end(), comp);
	if (iter != children.end()) {
		children.erase(iter);
	}
}

void CompositeComponent::ProceedInput(InputDevice* inpDevice)
{
	for (Component* child : children) {
		child->ProceedInput(inpDevice);
	}
}

void CompositeComponent::Initialize(Compositer* parent)
{
	for (Component* child : children) {
		child->Initialize(this);
	}
	ComputeWorldTransform();
}

void CompositeComponent::Update(float deltaTime, Compositer* parent)
{
	ComputeWorldTransform();
	for (Component* child : children) {
		child->Update(deltaTime, this);
	}
	ComputeWorldTransform();
}

void CompositeComponent::SetPosition(Math::Vector3 pos)
{
	position = pos;
	recomputeWorldTransform = true;
}

void CompositeComponent::SetRotation(Math::Quaternion rot)
{
	rotation = rot;
	recomputeWorldTransform = true;
}

void CompositeComponent::SetScale(Math::Vector3 scale)
{
	this->scale = scale;
	recomputeWorldTransform = true;
}

Math::Vector3 CompositeComponent::GetForward() const
{
	return Math::Vector3::Transform(Math::Vector3::UnitX, rotation);
}

Math::Vector3 CompositeComponent::GetRight() const
{
	return Math::Vector3::Transform(Math::Vector3::UnitY, rotation);
}

void CompositeComponent::ComputeWorldTransform()
{
	// TODO: use parent's world transform here
	if (!recomputeWorldTransform) {
		return;
	}

	recomputeWorldTransform = false;
	// Scale, then rotate, then translate
	worldTransform = Math::Matrix::CreateScale(scale);
	worldTransform *= Math::Matrix::CreateFromQuaternion(rotation);
	worldTransform *= Math::Matrix::CreateTranslation(position);
}

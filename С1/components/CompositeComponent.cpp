#include "CompositeComponent.h"

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
}

void CompositeComponent::Update(float deltaTime, Compositer* parent)
{
	for (Component* child : children) {
		child->Update(deltaTime, this);
	}
}

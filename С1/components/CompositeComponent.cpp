#include "CompositeComponent.h"

CompositeComponent::CompositeComponent(Game* game)
	: Component(game)
{
}

CompositeComponent::~CompositeComponent()
{
	for (Component* child : children) {
		delete child;
	}
	children.clear();
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

#pragma once

#include "core/Component.h"
#include "core/Compositer.h"

#include <vector>
#include <initializer_list>

class CompositeComponent : public Component, public Compositer {
public:
	CompositeComponent(Game* game, Compositer* compositer = nullptr);
	~CompositeComponent();

	void AddChild(const std::initializer_list<Component*>& batch) override { children.insert(children.end(), batch.begin(), batch.end()); }

	void ProceedInput(InputDevice* inpDevice) override;
	void Initialize(Compositer* parent = nullptr) override;
	void Update(float deltaTime, Compositer* parent = nullptr) override;

	Math::Vector2 GetPosition() const override { return position; }
	void SetPosition(Math::Vector2 pos) override { position = pos; }

protected:
	std::vector<Component*> children;

	Math::Vector2 position{0.0f, 0.0f};
};

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

	Math::Vector3 GetPosition() const override { return position; }
	void SetPosition(Math::Vector3 pos) override;

	Math::Quaternion GetRotation() const override { return rotation; }
	void SetRotation(Math::Quaternion rot) override;

	Math::Vector3 GetScale() const override { return scale; }
	void SetScale(Math::Vector3 scale) override;

	Math::Vector3 GetForward() const override;
	Math::Vector3 GetRight() const override;

	const Math::Matrix& GetWorldTransform() const override { return worldTransform; }
	void ComputeWorldTransform();

protected:
	std::vector<Component*> children;

	Math::Vector3 position = Math::Vector3::Zero;
	Math::Quaternion rotation = Math::Quaternion::Identity;
	Math::Vector3 scale{1.0f, 1.0f, 1.0f};

	Math::Matrix worldTransform;
	bool recomputeWorldTransform = true;
};

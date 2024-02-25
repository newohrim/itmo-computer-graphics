#pragma once

#include "Math.h"
#include <initializer_list>

template<typename CHILD_T>
class Compositer_T {
public:
	virtual void AddChild(const std::initializer_list<CHILD_T*>& batch) = 0;
	virtual Math::Vector2 GetPosition() const = 0;
	virtual void SetPosition(Math::Vector2 pos) = 0;
};

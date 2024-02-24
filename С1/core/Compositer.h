#pragma once

#include "Math.h"

class Compositer {
public:
	virtual DirectX::SimpleMath::Vector2 GetPosition() const = 0;
};

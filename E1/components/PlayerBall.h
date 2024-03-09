#pragma once

#include "CompositeComponent.h"

class PlayerBall : public CompositeComponent {
public:
	PlayerBall(Game* game);

	void Initialize(Compositer* parent = nullptr) override;
	void ProceedInput(InputDevice* inpDevice) override;
};

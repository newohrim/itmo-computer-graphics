#pragma once

class Game;
template<typename>
class Compositer_T;
using Compositer = Compositer_T<class Component>;

class Component
{
public:
	Component(Game* game, Compositer* parent = nullptr);
	virtual ~Component();

	virtual void Initialize(Compositer* parent = nullptr) {  }
	virtual void ProceedInput(class InputDevice* inpDevice) {  }
	virtual void Update(float deltaTime, Compositer* parent = nullptr) {  }

	Game* GetGame() const { return game; }

private:
	Game* game;
	bool hasParent = false;
};

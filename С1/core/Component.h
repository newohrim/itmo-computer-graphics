#pragma once

#include <SimpleMath.h>

class Game;
class Compositer;

class Component
{
public:
	Component(Game* game);
	virtual ~Component();

	virtual void Initialize(Compositer* parent = nullptr) {  };
	virtual void Update(float deltaTime, Compositer* parent = nullptr) {  };

	Game* GetGame() const { return game; }

private:
	Game* game;
};

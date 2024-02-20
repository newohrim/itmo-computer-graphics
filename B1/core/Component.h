#pragma once

class Game;

class Component
{
public:
	Component(Game* game);
	~Component();

	virtual void Initialize() {  };
	virtual void Update(float deltaTime) {  };

	Game* GetGame() const { return game; }

private:
	Game* game;
};

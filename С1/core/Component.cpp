#include "Component.h"

#include "Game.h"

Component::Component(Game* _game) 
	: game(_game)
{
	game->AddComponent(this);
}

Component::~Component()
{
	game->RemoveComponent(this);
}

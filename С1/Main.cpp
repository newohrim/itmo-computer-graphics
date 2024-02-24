#include "core/Game.h"

int main(int argc, char* argv[])
{
	Game game;
	bool success = game.Initialize("C1", 800, 800);
	if (success)
	{
		game.RunLoop();
	}
	game.Shutdown();

	return 0;
}

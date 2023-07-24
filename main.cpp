#include <Windows.h>
#include "Game.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Game* game;
	bool res;

	// Create the Game object
	game = new Game;

	// Initialize the game and run
	res = game->Initialize();
	if (res)
	{
		game->Run();
	}

	// Shutdown and release
	game->Shutdown();
	delete game;
	game = 0;

	return 0;
}
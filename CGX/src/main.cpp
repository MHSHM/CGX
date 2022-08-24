#include <Game.h>

int main() 
{
	Game game; 

	bool init = game.Init(); 

	if (init) 
	{
		game.Pre_Init();
		game.Run_Game(); 
	}

	game.Shutdown(); 

	return 0;
}
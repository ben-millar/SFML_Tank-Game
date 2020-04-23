#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#pragma comment(lib,"thor-d.lib")
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#pragma comment(lib,"thor.lib")
#endif 
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"glu32.lib") 

#pragma comment(lib,"libyaml-cppmdd")
#include "Game.h"

// KNOWN BUGS: Sometimes the top 1/4 of the screen will be filled by some kind of 'wood grain' texture. 
// This has happened twice in the several hundred times I've run the game, so I'm struggling to narrow down the cause.
// Perhaps the effect of some race condition; SFML may handle file loading asynchronously under the hood, and I may be
// assigning textures before they're fully loaded into memory.

/// <summary>
/// @brief starting point for all C++ programs.
/// 
/// Create a game object and run it.
/// </summary>
/// <param name=""></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int, char* argv[])
{
	srand(static_cast<unsigned>(time(nullptr)));

	Game game;
	game.run();
}
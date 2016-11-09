//Game.h
#ifndef GAME_INCLUDED
#define GAME_INCLUDED
///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////
class Arena;

class Game
{
public:
	// Constructor/destructor
	Game(int rows, int cols, int nRobots);
	~Game();

	// Mutators
	void play();

private:
	Arena* m_arena;
};
#endif

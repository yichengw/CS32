//Arena.h
#ifndef ARENA_INCLUDED
#define ARENA_INCLUDED
///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////
#include "globals.h"
#include "History.h"

class Robot;
class Player;


class Arena
{
public:
	// Constructor/destructor
	Arena(int nRows, int nCols);
	~Arena();

	// Accessors
	int     rows() const;
	int     cols() const;
	Player* player() const;
	int     robotCount() const;
	int     nRobotsAt(int r, int c) const;
	bool    determineNewPosition(int& r, int& c, int dir) const;
	void    display() const;

	// Mutators
	bool   addRobot(int r, int c);
	bool   addPlayer(int r, int c);
	bool   attackRobotAt(int r, int c, int dir);
	bool   moveRobots();

	History& history();

private:
	int     m_rows;
	int     m_cols;
	Player* m_player;
	Robot*  m_robots[MAXROBOTS];
	int     m_nRobots;
	History h;
};
#endif

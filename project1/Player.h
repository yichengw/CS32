//Player.h
#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED
///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Arena;  // This is needed to let the compiler know that Arena is a
			  // type name, since it's mentioned in the Robot declaration.
class Player
{
public:
	// Constructor
	Player(Arena *ap, int r, int c);

	// Accessors
	int  row() const;
	int  col() const;
	int  age() const;
	bool isDead() const;

	// Mutators
	void   stand();
	void   moveOrAttack(int dir);
	void   setDead();

private:
	Arena* m_arena;
	int    m_row;
	int    m_col;
	int    m_age;
	bool   m_dead;
};
#endif

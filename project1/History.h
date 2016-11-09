//History.h
#ifndef HISTORY_INCLUDED
#define HISTORY_INCLUDED
///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////
class History
{
public:
	History(int nRows, int nCols);
	bool record(int r, int c);
	void display() const;
private:
	int m_rows;
	int m_cols;
	char h_grid[20][30];

};
#endif

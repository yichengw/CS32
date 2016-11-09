#include "History.h"
#include "globals.h"

#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>
using namespace std;

History::History(int nRows, int nCols)
{
	// Fill the grid with dots
	m_rows = nRows;
	m_cols = nCols;
	int i, j;
	for (i = 0; i < m_rows; i++)
	{
		for (j = 0; j < m_cols; j++)
		{
			h_grid[i][j] = '.';
		}
	}
}
bool History::record(int r, int c)
{
	//The record function is to be called to notify the History object 
	//that a robot has died at a grid point in the Arena that the History object corresponds to. 
	//The function returns false if row r, column c is not within bounds; otherwise, it returns true 
	//after recording what it needs to. This function expects its parameters to be expressed in the same 
	//coordinate system as the Arena (e.g., row 1, column 1 is the upper-left-most position).
	int r_r = r-1, c_r = c-1;
	if (r_r < 0 || r_r >= m_rows || c_r < 0 || c_r >= m_cols)
	{
		return false;
	}
	else
	{
		if (h_grid[r_r][c_r] == '.')
		{
			h_grid[r_r][c_r] = 'A';
		}
		else if (h_grid[r_r][c_r] >= 'A' && h_grid[r_r][c_r] <= 'Y')
		{
			h_grid[r_r][c_r] ++;
		}
		else
		{
			h_grid[r_r][c_r] = 'Z';
		}
		return true;
	}
}

void History::display() const
{
	// Draw the grid of history
	clearScreen();
	int i, j;
	for (i = 0; i < m_rows; i++)
	{
		for (j = 0; j < m_cols; j++)
			cout << h_grid[i][j];
		cout << endl;
	}
	cout << endl;
}



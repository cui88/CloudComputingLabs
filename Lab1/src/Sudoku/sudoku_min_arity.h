#ifndef SUDOKU_MIN_ARITY_H
#define SUDOKU_MIN_ARITY_H

#include <assert.h>

#include <algorithm>
#include<string.h>

#include "sudoku.h"

struct Mina
{ 
    int board[N];
	int spaces[N];
	int neighbors[N][NEIGHBOR];
	int nspaces;
	int (*chess)[COL] = (int (*)[COL])board;
    
    Mina(char* in);
    void find_spaces();
    bool available(int guess, int cell);
    void input(const char in[]);
	int arity(int cell);
	void find_min_arity(int space);
	bool solve_sudoku_min_arity(int which_space);
    void mina_solve(int* solution);
};

#endif
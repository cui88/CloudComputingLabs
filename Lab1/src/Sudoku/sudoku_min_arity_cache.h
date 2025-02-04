#ifndef SUDOKU_MIN_ARITY_CACHE_H
#define SUDOKU_MIN_ARITY_CACHE_H

#include <assert.h>
#include <strings.h>

#include <algorithm>
#include <string.h>
#include "sudoku.h"

struct Minac
{
    int board[N];
	int spaces[N];
	int neighbors[N][NEIGHBOR];
	int nspaces;
	int (*chess)[COL] = (int (*)[COL])board;
    bool occupied[N][NUM+1];
    int arity[N];

    Minac(char* in);
    void input(const char in[]);
    void find_spaces();
    bool available(int guess, int cell);
    void find_min_arity(int space);
    void init_cache();
    bool solve_sudoku_min_arity_cache(int which_space);
    void minac_solve(int* solution);
};

#endif
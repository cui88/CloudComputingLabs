#ifndef SUDOKU_BASIC_H
#define SUDOKU_BASIC_H

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include <string.h>

using namespace std;

#include "sudoku.h"

struct Basic
{
	int board[N];
    int spaces[N];
    int neighbors[N][NEIGHBOR];
    int nspaces;
    int (*chess)[COL] = (int (*)[COL])board;
    
    Basic(char *in);
    void find_spaces();
    void input(const char in[N]);
    bool available(int guess, int cell);
    bool solve_sudoku_basic(int which_space);
    void basic_solve(int* solution);
};

#endif
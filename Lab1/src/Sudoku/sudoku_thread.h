#ifndef SUDOKU_THREAD_H
#define SUDOKU_THREAD_H

#include <queue>
#include <map>
#include <vector>
#include <pthread.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <sys/time.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <condition_variable>
#include <iomanip>

#include "sudoku_basic.h"
#include "sudoku_dancing_links.h"
#include "sudoku_min_arity.h"
#include "sudoku_min_arity_cache.h"

using namespace std;

#define IF_PRINT_DEBUG 1
#define _FUNC_TYPE int
#define _THRD_TYPE int
#define N 81

#define BASIC 0
#define DANCE 1
#define MINA  2 
#define MINAC 3

#define PUZZLE_ASSIGN_THRD 0
#define SUDOKU_SOLVE_THRD  1
#define PRINT_THRD         2
#define FILR_READ_THRD     3

struct Sudoku_puzzle_desc
{
	long int ith_puzzle;
	char sudoku_puzzle [90];
};

struct Sudoku_solution_desc
{
	long int ith_solution;
	int sudoku_solution [90];
};

/*struct Sudoku_queues 
{
	queue<struct Sudoku_puzzle_desc*> *puzzle_queue;
	queue<struct Sudoku_solution_desc*> *solution_queue;
};*/

struct File_steam_desc
{
	FILE *fp;
	char *f_buf;
	long int f_size;
};

struct Cv_lk{
    mutex mtx;
    condition_variable cv;
    bool ready ;
};


double now();
void set_start_time();
void set_end_time();
void print_consumed_time();
void init_func_type(int func_type);
void start_thrd(int func_type);
pthread_t create_print_sudoku_thrd();
void* print_sudoku_thrd(void* arg);
void thrd_exit_arbitrate();
void update_current_solve();
void printf_single_solution(int solution[]);
pthread_t create_solve_sudoku_thrd();
void* solve_sudoku_thrd(void* arg);
void put_solution_to_map(struct Sudoku_solution_desc* solution_desc);
struct Sudoku_solution_desc* sudoku_solve(struct Sudoku_puzzle_desc* puzzle_desc);
void get_solution_desc(char* puzzle,int* solution);
void get_basic_solution(char* puzzle,int* solution);
void get_dance_solution(char* puzzle,int* solution);
void get_mian_solution(char* puzzle,int* solution);
void get_minac_solution(char* puzzle,int* solution);
struct Sudoku_puzzle_desc* get_puzzle_from_queue();
void sudoku_read_file();
void create_print_file();
bool check_puzzle_queue_empty();
bool check_solution_map_empty();
bool check_print_empty();
void* read_file_thrd(void *arg);
void read_file(string filename);
void create_get_puzzle_thrd();
void* sudoku_puzzle_thrd(void* arg);
void get_sudoku_puzzle(struct Sudoku_puzzle_desc* puzzle_desc);
void wait_file_read(char* start,char** current);
struct Sudoku_puzzle_desc* get_next_puzzle_buf(char** current,char *start,char *end);
void ignore_line_break (char** current);
void update_current_ptr(char** current);
//void destroyed();
#endif
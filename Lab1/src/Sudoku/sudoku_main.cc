#include "sudoku_thread.h"

using namespace std;

/*
*Basic_Mode
*/

/* switch solving algorithm, option:BASIC/DANCE/MINA/MINAC */
_FUNC_TYPE DEFAULT_FUNC_TYPE = MINAC;    

int main()
{    
	start_thrd (DEFAULT_FUNC_TYPE);
    
    // sleep(1000);
	print_consumed_time (); 
	fflush(stdout);

	//destoyed();  
}
#include "sudoku_thread.h"

_FUNC_TYPE FUNC_TYPE;
//_THRD_TYPE THRD_TYPE;
queue<struct Sudoku_puzzle_desc*> sudoku_puzzle_queue;
//queue<struct Sudoku_solution_desc*> sudoku_solution_queue;
map<long int,struct Sudoku_solution_desc*> sudoku_solution_map;
struct File_steam_desc* file_steam_desc;

map<string,struct Cv_lk*> cv_map =
{
  {"puzzle_thrd", new Cv_lk()},
  {"solution_thrd",new Cv_lk()},
  {"print_thrd",new Cv_lk()},
};

bool sodoku_file_done    =false;
bool sodoku_solve_done   =false;
bool sodoku_puzzle_done  =false;
bool soduku_print_done   =false;
bool showing_terminal_on =false;

long int total_solve = 0;
long int total_puzzle = 0;
long int current_solve = 1;
long int total_sodoku = 0;

double start_time;
double end_time;

double now()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void set_start_time()
{
	start_time = now();
}

void set_end_time()
{
	end_time = now();
}

void print_consumed_time ()   
{
	/* seconds */
  double total_time = (end_time - start_time) / 1000000.0;

	cout << "Total Sudoku puzzles solved: " << total_puzzle  << "; total consumed time: " << total_time << " seconds; each consumed time: " << total_time / total_puzzle  << " seconds." << endl;  
}

void init_func_type(int func_type)
{
    FUNC_TYPE = func_type;
}

void start_thrd(int func_type)
{
	int nprocs = get_nprocs();
  int join_num = nprocs;
  if(IF_PRINT_DEBUG)
    cout << "nprocs :" << nprocs <<endl;  
    int s = 0;
   // int p = 0;
	pthread_t thread_solve[nprocs];
	pthread_t thread_print;

	init_func_type(func_type);
    //struct* Sudoku_queues queue = NULL;
    //init_struct_queue(queue);
	//create_print_file();
    //read file
	sudoku_read_file();
	create_get_puzzle_thrd();

	while(nprocs -- )
	{
		thread_solve[s] = create_solve_sudoku_thrd();
		s++; 
	}
    thread_print = create_print_sudoku_thrd();
    
    for(int i = 0; i < join_num; i ++)
    {
       //
       pthread_join(thread_solve[i],NULL);
       //printf("%s %d\n", "thread id: ", thread_solve[i]);
       //
    }
    pthread_join(thread_print,NULL);
    set_end_time();
    printf("%s\n", "over..............");
}

pthread_t create_print_sudoku_thrd()
{
	pthread_t thread;

	int ret = pthread_create(&thread,NULL,print_sudoku_thrd,NULL);
	return thread;
}

void* print_sudoku_thrd(void* arg)
{
	map<long int,struct Sudoku_solution_desc*>::iterator iter;
    //open_showing_terminal();
	while(true)
  {
    unique_lock<std::mutex> lck2(cv_map["print_thrd"]->mtx);
    while(!(cv_map["print_thrd"]->ready))
      cv_map["print_thrd"]->cv.wait(lck2);
    while(check_solution_map_empty())
    {
      cout << "wait for solution map..." << endl;
      if(soduku_print_done)
      {
        lck2.unlock();
        cout << "print_sudoku_thrd exit!" << endl;
        thrd_exit_arbitrate();
      }
    }
    
    iter = sudoku_solution_map.find(current_solve);
    if(iter != sudoku_solution_map.end())
    {
        //printf_single_solution(iter->second->sudoku_solution);
        update_current_solve();
        sudoku_solution_map.erase(iter);
    } 
    lck2.unlock();
  }
}


/*void thrd_exit_arbitrate(int thrd_type)
{

    switch (THRD_TYPE)
    {
    	case FILE_READ_THRD
    	   if( ! sodoku_file_done)
    	   	  pthread_exit(NULL);
    	   	break;
    	case PUZZLE_ASSIGN_THRD
    	   if( ! sodoku_puzzle_done)
              pthread_exit(NULL);
    	   break;
    	case SUDOKU_SOLVE_THRD
    	   if( ! sodoku_solve_done)
    	   	  pthread_exit(NULL);
    	   break;
    	case PRINT_THRD
    	   if( ! sodoku_print_done)
    	   	  pthread_exit(NULL);
    	   break;
    }
}*/
void thrd_exit_arbitrate()
{
    pthread_exit(NULL);
}

/*pthread_mutex terminal_mux;
void open_showing_terminal()
{
	pthread_mutex_lock (&terminal_mux);
	if (!showing_terminal_on)
	{
		system ("gnome-terminal --title='Hey, this is a terminal for showing the sudoku solution!' -- sh -c 'tail -f ./sudoku_solution.out; exex bash'");
		cout << "Ready for displaying results. Please enter file name at another terminal and wait......" << endl;
		showing_terminal_on = true;
	}
	pthread_mutex_unlock(&terminal_mux);
}*/

void update_current_solve()
{
	current_solve ++;
  if(current_solve >= total_sodoku)
    soduku_print_done = true;
}

void printf_single_solution(int solution[])
{
    for(int i = 0; i < N; i++ )
        cout << solution[i];
    cout << endl;
}

pthread_t create_solve_sudoku_thrd()
{
	pthread_t thread;

	int ret = pthread_create(&thread,NULL,solve_sudoku_thrd,NULL);
	return thread;
}

void* solve_sudoku_thrd(void* arg)
{
	struct Sudoku_puzzle_desc* puzzle = NULL;
	struct Sudoku_solution_desc* solution_desc = NULL;
    while(true)
    {
       unique_lock<std::mutex> lck1(cv_map["solution_thrd"]->mtx);
       while(!(cv_map["solution_thrd"]->ready))
           cv_map["solution_thrd"]->cv.wait(lck1);
       while(!(puzzle = get_puzzle_from_queue()))
       {
           //cout << "wait puzzle..." << endl;
           if( sodoku_solve_done )
           {  
              lck1.unlock();
              cv_map["solution_thrd"]->cv.notify_one();
              //cout << "sodoku_solve_done" << sodoku_solve_done << endl;
              thrd_exit_arbitrate();
           }
       }
       lck1.unlock();
       cv_map["solution_thrd"]->cv.notify_one();
       solution_desc = sudoku_solve(puzzle);
       /*if(IF_PRINT_DEBUG){
         cout << "solve_sudoku_thrd";
         cout << "ith_solution :" << solution_desc->ith_solution << endl;
         printf_single_solution(solution_desc->sudoku_solution);
       }*/
       put_solution_to_map(solution_desc);   
    }
    printf("%s\n", "solve thread exit!");
}

pthread_mutex_t ps_mtx;

void put_solution_to_map(struct Sudoku_solution_desc* solution_desc)
{

	  pthread_mutex_lock(&ps_mtx);
    sudoku_solution_map.insert(std::make_pair (solution_desc->ith_solution, solution_desc));
    total_solve ++;
    if(total_solve >= total_sodoku)
      sodoku_solve_done = true;
    cv_map["print_thrd"]->ready = true;
    cv_map["print_thrd"]->cv.notify_one();
    pthread_mutex_unlock(&ps_mtx);
}

struct Sudoku_solution_desc* sudoku_solve(struct Sudoku_puzzle_desc* puzzle_desc)
{
    struct Sudoku_solution_desc* solution_desc = new Sudoku_solution_desc();
    get_solution_desc(puzzle_desc->sudoku_puzzle,solution_desc->sudoku_solution);
    solution_desc->ith_solution = puzzle_desc->ith_puzzle;
    return solution_desc;
}

void get_solution_desc(char* puzzle,int* solution)
{  
	switch (FUNC_TYPE)
	{
		case BASIC:
		   get_basic_solution(puzzle,solution);
		   break;
		case DANCE:
		   get_dance_solution(puzzle,solution);
		   break;
		case MINA:
		   get_mian_solution(puzzle,solution);
		   break;
		case MINAC:
		   get_minac_solution(puzzle,solution);
		   break;	
	} 
}

void get_basic_solution(char* puzzle,int* solution)
{
   Basic b (puzzle);
   b.basic_solve(solution);
}

void get_dance_solution(char* puzzle,int* solution)
{
   Dance d (puzzle);
   d.dance_solve(solution);
}

void get_mian_solution(char* puzzle,int* solution)
{
   Mina m (puzzle);
   m.mina_solve(solution);
}

void get_minac_solution(char* puzzle,int* solution)
{
   Minac c (puzzle);
   c.minac_solve(solution);
}

struct Sudoku_puzzle_desc* get_puzzle_from_queue()
{
	if(check_puzzle_queue_empty())//队列为空且取puzzle未完成
	{
    if(IF_PRINT_DEBUG)
      //cout << "puzzle_queue_empty!" << endl;
    return NULL;
  }	
	else 
	{
    struct Sudoku_puzzle_desc* puzzle_desc = new Sudoku_puzzle_desc ();
		//cout << "sudoku_puzzle_queue:" << sudoku_puzzle_queue.front()->sudoku_puzzle << endl;
    memcpy(puzzle_desc,sudoku_puzzle_queue.front(),sizeof(struct Sudoku_puzzle_desc));
		//delete sudoku_puzzle_queue.front();
    cout << "copy after:" << puzzle_desc->sudoku_puzzle << endl;
    sudoku_puzzle_queue.pop(); 
    if(IF_PRINT_DEBUG)
      cout << "sudoku_puzzle_queue_size: " << sudoku_puzzle_queue.size() << endl;
	  return puzzle_desc;
  }
}

bool check_puzzle_queue_empty()
{
	if(sudoku_puzzle_queue.empty())
		return true;
	else return false;
}

bool check_solution_map_empty()
{
	if( sudoku_solution_map.empty() )
	    return true;
	else return false;
}

bool check_print_empty()
{
    if(check_solution_map_empty() && soduku_print_done)
    	return true;
    else false;
}

/*void create_print_file()
{
	freopen("./solve_sudoku.out","w",stdout);
}*/

void sudoku_read_file()
{
	pthread_t thread;
	string* filename = new string();
	std::cin >> (*filename);
  fflush(stdin);
  if(IF_PRINT_DEBUG)
    cout << "sudoku_read_file" << endl;
	set_start_time();
  int ret = pthread_create(&thread,NULL,read_file_thrd,(void*)filename);
  assert (ret == 0);
}

void* read_file_thrd(void *arg)
{
  string* filename = (string*) arg;
  read_file(*filename);
}

void read_file(string filename)
{
	  long int lsize;
	  size_t result;
    int i = 0;
    while(i<filename.length())
    {
      if( filename[i] <='9' && filename[i] >='0')
         total_sodoku = total_sodoku * 10 + filename[i] - '0';
      i++;
    }
    if(IF_PRINT_DEBUG)
       cout << "total_sodoku:" << total_sodoku << endl;
	  file_steam_desc = new File_steam_desc();
    FILE* fp = NULL;
    if((fp = fopen(&filename[0],"rb")) == NULL)
    {
    	cout << filename << "File error!" << endl;
    } 
    file_steam_desc->fp = fp;

    /*get file size*/
    fseek(fp, 0, SEEK_END);
    lsize = ftell(fp);
    rewind(fp);
    
    file_steam_desc->f_size = lsize; 
    //cout <<  "file_steam_desc->f_size :" << file_steam_desc->f_size << endl;  
    file_steam_desc->f_buf = (char*)malloc(sizeof(char)*(file_steam_desc->f_size));
    if(file_steam_desc->f_buf == NULL)
    {
    	cout << "Memory error!" << endl;
    }
    
    cv_map["puzzle_thrd"]->ready = true;
    cv_map["puzzle_thrd"]->cv.notify_one();
    result = fread(file_steam_desc->f_buf,lsize,1,fp);
    if(IF_PRINT_DEBUG)
      cout <<"result:" << result  << "   lsize:" << lsize <<endl;
    fflush(stdout);
    if(result < lsize)
    {
    	cout << "Reading error!" << endl;
    }
    else
    {
      //fclose(fp);
    	cout << "read over!" << endl;
    	sodoku_file_done = true;
    	thrd_exit_arbitrate();
    }
}

void create_get_puzzle_thrd()
{
	pthread_t thread;
	//if buffer is null,cannot get_puzzle
    int ret = pthread_create(&thread,NULL,sudoku_puzzle_thrd,NULL);
    assert( ret == 0);
}

/*Struct* Sudoku_queues init_struct_queue(struct* Sudoku_queues queue)
{
	puzzle_queue = new Sudoku_puzzle_desc();
	solution_queue = new Sudoku_solution_desc();
	return queue;
}*/

void* sudoku_puzzle_thrd(void* arg)
{
	//struct* Sudoku_queues sudoku_queue= (struct* Sudoku_queues)arg;
	struct Sudoku_puzzle_desc* puzzle_desc = new Sudoku_puzzle_desc();
	//struct* Sudoku_puzzle_desc solution_desc = NULL;
   
    while(true)
    {
       if(sodoku_puzzle_done){
         cout << "puzzle done" << endl;
         thrd_exit_arbitrate();
       }
       unique_lock<std::mutex> lck3(cv_map["puzzle_thrd"]->mtx);
       while(!(cv_map["puzzle_thrd"]->ready))
       {
          cout << "wait for puzzle thread!" << endl;
          cv_map["puzzle_thrd"]->cv.wait(lck3);
       }
       
       //cv_map["puzzle_thrd"]->ready = false;
       get_sudoku_puzzle(puzzle_desc);

       lck3.unlock();
    }
}

void get_sudoku_puzzle(struct Sudoku_puzzle_desc* puzzle_desc)
{
    struct Sudoku_puzzle_desc* puzzle = puzzle_desc;
    char *start = file_steam_desc->f_buf;
    char *current = start;
    char *end = start + (file_steam_desc->f_size - 1);
    //cout << "*end:" << hex << end << endl;
    
    while(puzzle = get_next_puzzle_buf(&current, start, end))
    {
        sudoku_puzzle_queue.push(puzzle);
        update_current_ptr(&current);
        cv_map["solution_thrd"]->ready = true;
        cv_map["solution_thrd"]->cv.notify_one();
    }
}

void wait_file_read(char* start,char** current)
{
  //cout << "current_now:" << (*current - start +81 ) << endl;
  //cout << "current file ptr:" << ftell(file_steam_desc->fp) << endl;
	while((*current - start + 81) >= ftell(file_steam_desc->fp))
	{
		cout << "wait read file...." << endl;
    continue;
	}
}

struct Sudoku_puzzle_desc* get_next_puzzle_buf(char** current,char *start,char *end)
{
    //if(IF_PRINT_DEBUG)
      //cout << "*end :" << end << "**current :" << *current <<"end -*current :"<< end - *current << endl;
    if(*current ==  end || *current == end -1)
    {
      cout << "puzzle done!" << endl;
    	sodoku_puzzle_done = true;
      delete file_steam_desc;
    	return NULL;
    }
	  wait_file_read(start,current);
    struct Sudoku_puzzle_desc* puzzle = new Sudoku_puzzle_desc();
    if(*current != start)
    {
    	ignore_line_break(current);
    }
    memcpy(puzzle->sudoku_puzzle,*current,N);
    if(puzzle == NULL)
    {
    	cout << "copy puzzle faild" << endl;
    }
    else{
      cout << "copy success!" << endl;
    }
    total_puzzle ++;
    puzzle->ith_puzzle = total_puzzle;
    return puzzle;
}

void ignore_line_break (char** current)
{
    *current = *current + 2;
}

void update_current_ptr(char** current)
{
  cout << "before current: " << static_cast<const void *>(*current) << endl;
	*current = *current + N - 1;
  cout << "after current: " <<  static_cast<const void *>(*current) << endl;
}

void destroyed()
{
    
}
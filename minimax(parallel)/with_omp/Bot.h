#define _Table_Size 8
#define Black 1
#define White -1
#define THREAD_NUM 8

#include <iostream>
#include <vector>

struct M_Block;
struct Node;
struct Path;
class Bot;

struct Node
{
    int value;
    int depth;
    int a;
    int b;
    int x;
    int y;
    int child_count;
    Node *parent;
    std::vector<Node*> child;
	short direct;
    bool move_jump;
};

struct M_Block
{
    int num = 0;
    Node nodes [10000];
    M_Block *next = NULL;
};

void *thread_func(void *);

class Bot
{
    public:
        Bot (short int, int);

        void load_param ();
        bool start (short int[_Table_Size][_Table_Size], double *);				//start minimax
        int Max (Node *, int);
        int Mini (Node *, int);
		friend void *thread_func(void *);
        
        Node *do_move_jump (Node *, Node *, int, int, int, int, int);			    //generate childs
        int cal_value (Node *, bool, int);								//calulate the value of node
        
        bool can_move (int, int, int, int, int);						//detect if the given chess can move
        bool can_jump (int, int, int, int, int);						//detect if the given chess can jump
        bool is_chess (int, int, int);									//detect if the given position is a chess
        bool in_bound (int, int);										//detect if the given position is in bound
        void get_board (Node *, bool, int, int);						//get the board of the node
        void shuffle_order (short int[4], int);								//shuffle the move order
        void shuffle_chess (int[9][2], int, int);
        bool print (Node *, short int[_Table_Size][_Table_Size]);
        bool end_game ();
        
		void delete_node (Node *);

        long long int node_count [THREAD_NUM][64];

        ~Bot ();
    private:
    	int round;
        short int b_or_w;												//this bot is black or white
        short int board [_Table_Size][_Table_Size];						//current board
        short int board_t [THREAD_NUM][_Table_Size][_Table_Size];					//temperary board
        short int weight[_Table_Size] = { 0,10,18,24,28,30,36,42};
        int seed [THREAD_NUM][64];              //padding
        M_Block *head [THREAD_NUM][64];
        M_Block *now [THREAD_NUM][64];
        int Minimax_Depth;
        Node *root;
};

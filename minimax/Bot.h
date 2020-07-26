#define _Table_Size 8
#define Black 1
#define White -1
#include <iostream>
#include <vector>

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
    std::vector<Path*> path;
};

struct Path
{
    int x, y, direct;
    bool move_jump;
};

class Bot
{
    public:
        Bot (short int, int);

        void load_param ();
        bool start (short int[_Table_Size][_Table_Size]);				//start minimax
        int Max (Node *);
        int Mini (Node *);
        
        Node *do_move (Node *, int, int, int);							//generate childs
        Node *do_jump (Node *, Node *, bool, int, int, int);			//generate childs
        int cal_value (Node *, bool);									//calulate the value of node
        
        bool can_move (int, int, int, int);								//detect if the given chess can move
        bool can_jump (int, int, int, int);								//detect if the given chess can jump
        bool is_chess (int, int);										//detect if the given position is a chess
        bool in_bound (int, int);										//detect if the given position is in bound
        void get_board (Node *, bool);									//get the board of the node
        void shuffle_order (short int[4]);								//shuffle the move order
        void shuffle_chess (int[9][2], int);
        bool print (Node *, short int[_Table_Size][_Table_Size]);
        bool end_game ();
        
        ~Bot ();
    private:
    	int round;
        short int b_or_w;												//this bot is black or white
        short int board [_Table_Size][_Table_Size];						//current board
        short int board_t [_Table_Size][_Table_Size];					//temperary board
        short int weight[_Table_Size] = { 0,10,18,24,28,30,36,42};
        int Minimax_Depth;
        Node *root;
};

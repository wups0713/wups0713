#define _Table_Size 8
#define Black 1
#define White -1
#include <iostream>
#include <vector>

struct Node_s;
struct Path_s;
class Bot_s;

struct Node_s
{
	int value;
	int depth;
	int a;
	int b;
	int x;
	int y;
	int child_count;
	Node_s *parent;
	std::vector<Node_s*> child;
	short direct;
    bool move_jump;
};

class Bot_s
{
public:
	Bot_s(short int, int);

	void load_param();
	bool start(short int[_Table_Size][_Table_Size], double *);				//start minimax
	int Max(Node_s *);
	int Mini(Node_s *);

	Node_s *do_move_jump(Node_s *, Node_s *, int, int, int, int);		//generate childs
	int cal_value(Node_s *, bool);									//calulate the value of node

	bool can_move(int, int, int, int);								//detect if the given chess can move
	bool can_jump(int, int, int, int);								//detect if the given chess can jump
	bool is_chess(int, int);										//detect if the given position is a chess
	bool in_bound(int, int);										//detect if the given position is in bound
	void get_board(Node_s *, bool, int);							//get the board of the node
	void shuffle_order(short int[4]);								//shuffle the move order
	void shuffle_chess(int[9][2], int);
	bool print(Node_s *, short int[_Table_Size][_Table_Size]);
	bool end_game();

	void delete_node(Node_s *);

	long long int node_count;

	~Bot_s();
private:
	int round;
	short int b_or_w;												//this bot is black or white
	short int board[_Table_Size][_Table_Size];						//current board
	short int board_t[_Table_Size][_Table_Size];					//temperary board
	short int weight[_Table_Size] = { 0,10,18,24,28,30,36,42 };
	int Minimax_Depth;
	Node_s *root;
};

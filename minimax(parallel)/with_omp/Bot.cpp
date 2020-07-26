#define _Table_Size 8
#define _Chess_Weight 100
#define Black 1
#define White -1
#define POS_INF 1000000
#define NEG_INF -1000000
#define THREAD_NUM 8
#define MULTI_THREAD 0			//0->on	1->off

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include "Bot.h"

short int up[2] = {0, -1};
short int down[2] = {0, 1};
short int left[2] = {-1, 0};
short int right[2] = {1, 0};

short int *dir[4] = {up, down, left, right};

Bot *bot_tmp;
pthread_t thread[THREAD_NUM];
int MULTITHREAD_ON_OFF;

Bot::Bot (short int bw, int depth)
{
	srand(time(NULL));

	round = 0;
	
    //set bot's color
    b_or_w = bw;

    //set minimax's depth
    Minimax_Depth = depth;
	MULTITHREAD_ON_OFF = (MULTI_THREAD) ? depth : 0;

    root = new Node;
}

Bot::~Bot ()
{
	// delete_node(root);
}

void Bot::delete_node(Node *node)
{
	for (int i = 0; i < node->child_count; i++)
	{
		delete_node(node->child[i]);
		delete node->child[i];
	}

	node->child.clear();
	node->child.shrink_to_fit();
}

void Bot::load_param ()
{

}

bool Bot::start (short int board_[_Table_Size][_Table_Size], double *total)
{
	round++;

	struct timeval t_start;
    struct timeval t_end;
    double time;

	gettimeofday (&t_start, NULL);
	    	
    //copy table
    for (int i = 0; i < _Table_Size; i++)
        for (int j = 0; j < _Table_Size; j++)
            board[i][j] = board_[i][j];
    
    for (int i = 0; i < THREAD_NUM; i++)
    {
        head [i][0] = new M_Block;
        now [i][0] = head [i][0];
    }

    //initialize root
    root->value = NEG_INF;
    root->depth = 0;
    root->a = NEG_INF;
    root->b = POS_INF;
    root->child_count = 0;
    root->parent = NULL;
    
    //start minimax
	bot_tmp = this;
    int solution = Max (root, 0);
    Node *node = NULL;
    
    //search for solution
    for (int i = 0; i < root->child_count; i++)
        if (root->child[i]->value == solution)
            if (node == 0 || cal_value (node, 0, 0) < cal_value (root->child[i], 0, 0))
                node = root->child[i];

	bool end = print (node, board_);

    gettimeofday (&t_end, NULL);
    time = t_end.tv_sec - t_start.tv_sec + (double)(t_end.tv_usec - t_start.tv_usec) / (double)1000000;
	std::cout << "\n\t\t" << time << " sec\n";

	*total += time;

    for (int i = 1; i < THREAD_NUM; i++)
        node_count [0][0] += node_count [i][0];
    node_count [0][0] += root->child_count;

    for (int i = 0; i < THREAD_NUM; i++)
    {
        M_Block *tmp = head [i][0];
        while (tmp)
        {
            M_Block *del_block = tmp;
            tmp = tmp->next;
            delete del_block;
        }
    }

	return end;
}

int Bot::Max (Node *node, int num)
{
    int v = NEG_INF, v_t;
    short int move_order[4] = {0, 1, 2, 3};
    short int jump_order[4] = {0, 1, 2, 3};
    int chess[9][2];
    int chess_count = 0;
    
    shuffle_order (move_order, num);
    shuffle_order (jump_order, num);
    
    node->child_count = 0;

    //if is end, return value
    if (node->depth == Minimax_Depth)
        return cal_value (node, 1, num);

    get_board (node, 1, num, node->depth);

    for (int y = 0; y < _Table_Size; y++)
        for (int x = 0; x < _Table_Size; x++)
        	if (board_t[num][y][x] == b_or_w)
			{
				chess[chess_count][0] = x;
				chess[chess_count][1] = y;
				chess_count++;
			}

	shuffle_chess (chess, chess_count, num);

    for (int k = 0; k < chess_count; k++)
    {
    	int x = chess[k][0];
    	int y = chess[k][1];
    	
        //move up, down, left, right
        for (int i = 0; i < 4; i++)
            if (can_move (x, y, dir[move_order[i]][0], dir[move_order[i]][1], num))
            {
				if (node->depth != MULTITHREAD_ON_OFF)		//on -> 0	off -> depth(always in)
				{
                    node_count [num][0]++;
					v_t = Mini(do_move_jump(node, NULL, 0, x, y, move_order[i], num), num);
					v = (v > v_t) ? v : v_t;

					if (v >= node->b)
					{
						node->value = v;
						return node->value;
					}
					node->a = (node->a > v) ? node->a : v;
				}
				else
					do_move_jump(node, NULL, 0, x, y, move_order[i], num);

                get_board (node, 1, num, node->depth);
            }

        //the node that try to keep jumping
        int start = node->child_count;
        Node *node_now = node;

        //position of moving chess
        int x_t = x;
        int y_t = y;

        //clear the path(-2), for bfs
        bool clear_path = 1;

        while (1)
        {
            //jump up, down, left, right
            for (int i = 0; i < 4; i++)
                if (can_jump (x_t, y_t, dir[jump_order[i]][0], dir[jump_order[i]][1], num))
                {
					if (node->depth != MULTITHREAD_ON_OFF)			//on -> 0	off -> depth(always in)
					{
                        node_count [num][0]++;
						v_t = Mini(do_move_jump(node, node_now, 1, x_t, y_t, jump_order[i], num), num);
						v = (v > v_t) ? v : v_t;

						if (v >= node->b)
						{
							node->value = v;
							return node->value;
						}
						node->a = (node->a > v) ? node->a : v;
					}
					else
						do_move_jump(node, node_now, 1, x_t, y_t, jump_order[i], num);

                    get_board (node_now, clear_path, num, node_now->depth);
                }

            //if no more child, then break
            if (start < node->child_count)
            {
                node_now = node->child[start++];
				x_t = node_now->x + 2 * dir [node_now->direct][0];
				y_t = node_now->y + 2 * dir [node_now->direct][1];

                clear_path = 0;

                get_board (node_now, 0, num, node_now->depth);
            }
            else
                break;
        }
        
        get_board (node, 1, num, node->depth);
    }

    //if no child, return value
    if (node->child_count == 0)
        return cal_value (node, 1, num);

	if (node->depth == MULTITHREAD_ON_OFF)			//on -> 0	off -> depth(never in)
	{
        int tmp [THREAD_NUM];

        omp_set_num_threads (THREAD_NUM);
        #pragma omp parallel
        {
            int a = NEG_INF, b = POS_INF, v_ = NEG_INF, v_t;
            int id = omp_get_thread_num ();

            seed [id][0] = time (NULL) + id;
            node_count [id][0] = 0;

            for (int i = id; i < node->child_count; i = i + THREAD_NUM)
            {
                node->child[i]->a = a;
                node->child[i]->b = b;

                v_t = bot_tmp->Mini(node->child[i], id);
                v_ = (v_ > v_t) ? v_ : v_t;

                if (v_ >= b)
                {
                    tmp [id] = v_;
                    break;
                }

                a = (a > v_) ? a : v_;
                tmp [id] = v_;
            }
        }

        for (int i = 0; i < THREAD_NUM; i++)
            v = (v > tmp [i]) ? v : tmp [i];
	}

    node->value = v;
    return node->value;
}

int Bot::Mini (Node *node, int num)
{
    int v = POS_INF, v_t;
    short int move_order[4] = {0, 1, 2, 3};
    short int jump_order[4] = {0, 1, 2, 3};
    int chess[9][2];
    int chess_count = 0;
    
    shuffle_order (move_order, num);
    shuffle_order (jump_order, num);
    
    node->child_count = 0;

    //if is end, return value
    if (node->depth == Minimax_Depth)
        return cal_value (node, 1, num);

    get_board (node, 1, num, node->depth);

    for (int y = 0; y < _Table_Size; y++)
        for (int x = 0; x < _Table_Size; x++)
        	if (board_t[num][y][x] == -b_or_w)
			{
				chess[chess_count][0] = x;
				chess[chess_count][1] = y;
				chess_count++;
			}

	shuffle_chess (chess, chess_count, num);

    for (int k = 0; k < chess_count; k++)
    {
    	int x = chess[k][0];
    	int y = chess[k][1];
    	
        //move up, down, left, right
        for (int i = 0; i < 4; i++)
            if (can_move (x, y, dir[move_order[i]][0], dir[move_order[i]][1], num))
            {
                node_count [num][0]++;
                v_t = Max (do_move_jump(node, NULL, 0, x, y, move_order[i], num), num);
                v = (v < v_t)?v:v_t;

                if (v <= node->a && node->depth > 1)
                {
                    node->value = v;
                    return node->value;
                }
                node->b = (node->b < v)?node->b:v;

                get_board (node, 1, num, node->depth);
            }

        //the node that try to keep jumping
        int start = node->child_count;
        Node *node_now = node;

        //position of moving chess
        int x_t = x;
        int y_t = y;

        //clear the path(-2), for bfs
        bool clear_path = 1;

        while (1)
        {
            //jump up, down, left, right
            for (int i = 0; i < 4; i++)
                if (can_jump (x_t, y_t, dir[jump_order[i]][0], dir[jump_order[i]][1], num))
                {
                    node_count [num][0]++;
                    v_t = Max (do_move_jump (node, node_now, 1, x_t, y_t, jump_order[i], num), num);
                    v = (v < v_t)?v:v_t;

                    if (v <= node->a && node->depth > 1)
                    {
                        node->value = v;
                        return node->value;
                    }
                    node->b = (node->b < v)?node->b:v;

                    get_board (node_now, clear_path, num, node_now->depth);
                }

            //if no more child, then break
            if (start < node->child_count)
            {
                node_now = node->child[start++];
				x_t = node_now->x + 2 * dir [node_now->direct][0];
				y_t = node_now->y + 2 * dir [node_now->direct][1];

                clear_path = 0;

                get_board (node_now, 0, num, node_now->depth);
            }
            else
                break;
        }
        
        get_board (node, 1, num, node->depth);
    }

    //if no child, return value
    if (node->child_count == 0)
        return cal_value (node, 1, num);

    node->value = v;
    return node->value;
}

Node *Bot::do_move_jump (Node *parent, Node *node, int move_jump, int x, int y, int direct, int num)
{
    //increase vector size if needed
    if (parent->child.size () < parent->child_count + 1)
        parent->child.resize (parent->child.size () + 5);

    //create node if had not been created
    Node *tmp;
    int t = now [num][0]->num;
    if (t == 10000)
    {
        M_Block *new_block = new M_Block;
        now [num][0]->next = new_block;
        now [num][0] = new_block;
        t = 0;
    }

    tmp = &now [num][0]->nodes [t];
    now [num][0]->num++;
    parent->child [parent->child_count] = tmp;

	//set data
	tmp->depth = parent->depth + 1;
	tmp->a = parent->a;
	tmp->b = parent->b;
	tmp->parent = (move_jump) ? node : parent;

	//set path
	tmp->x = x;
	tmp->y = y;
	tmp->direct = direct;
	tmp->move_jump = move_jump;

	//add count
	(parent->child_count)++;

	return tmp;
}

int Bot::cal_value (Node *node, bool rewrite, int num)
{
    int value = 0;

    get_board (node, 1, num, node->depth);
    
    for (int y = 0; y < _Table_Size; y++)
        for (int x = 0; x < _Table_Size; x++)
        {
        	if (b_or_w == Black)
        	{
	            if (board_t[num][y][x] == Black)
	                value += weight[x] + _Chess_Weight - 10;
	            if (board_t[num][y][x] == White)
	                value += -weight[_Table_Size - x - 1] - _Chess_Weight;
			}
			else
			{
				if (board_t[num][y][x] == White)
	                value += weight[_Table_Size - x - 1] + _Chess_Weight - 10;
	            if (board_t[num][y][x] == Black)
	                value += -weight[x] - _Chess_Weight;
			}
        }

    if (rewrite)
	    node->value = value;

    return value;
}

bool Bot::can_move (int x, int y, int d_x, int d_y, int num)
{
    if (in_bound (x + d_x, y + d_y) && !is_chess (x + d_x, y + d_y, num))
        return true;
    return false;
}

bool Bot::can_jump (int x, int y, int d_x, int d_y, int num)
{
    if (in_bound (x + 2 * d_x, y + 2 * d_y) && is_chess (x + d_x, y + d_y, num) && board_t[num][y + 2 * d_y][x + 2 * d_x] == 0)
        return true;
    return false;
}

bool Bot::is_chess (int x, int y, int num)
{
    if (board_t[num][y][x] == b_or_w || board_t[num][y][x] == -b_or_w)
        return true;
    return false;
}

bool Bot::in_bound (int x, int y)
{
    if (x >= 0 && x < _Table_Size && y >= 0 && y < _Table_Size)
        return true;
    return false;
}

void Bot::get_board (Node *node, bool clear_path, int num, int depth)
{
    if (node->parent == NULL)
    {
    	for (int i = 0; i < _Table_Size; i++)
        	for (int j = 0; j < _Table_Size; j++)
            	board_t[num][i][j] = board[i][j];
        return;
	}

	//get path from root to node
	if (node->depth == depth)
		get_board(node->parent, clear_path, num, depth);
	else
		get_board(node->parent, 1, num, depth);

    int x = node->x, y = node->y;
    int d_x = dir[node->direct][0], d_y = dir[node->direct][1];
    
    if (node->move_jump)
    {
        //jump
        board_t[num][y + 2 * d_y][x + 2 * d_x] = board_t[num][y][x];
        if (board_t[num][y + d_y][x + d_x] == -board_t[num][y][x])
            board_t[num][y + d_y][x + d_x] = 0;
        if (clear_path)
            board_t[num][y][x] = 0;
        else
            board_t[num][y][x] = -2;
    }
    else
    {
        //move
        board_t[num][y + d_y][x + d_x] = board_t[num][y][x];
        board_t[num][y][x] = 0;
    }
}

void Bot::shuffle_order (short int order[4], int num)
{
	for (int i = 0; i < 4; i++)
	{
        seed [num][0] = (214013 * seed [num][0] + 2531011);
		int exchange = ((seed [num][0] >> 16) & 0x7FFF) % 4;
		int tmp = order[i];
		order[i] = order[exchange];
		order[exchange] = tmp;
	}
}

void Bot::shuffle_chess (int chess[9][2], int count, int num)
{
	for (int i = 0; i < count; i++)
	{
        seed [num][0] = (214013 * seed [num][0] + 2531011);
		int exchange = ((seed [num][0] >> 16) & 0x7FFF) % count;
		int tmp1 = chess[i][0];
		int tmp2 = chess[i][1];
		chess[i][0] = chess[exchange][0];
		chess[i][1] = chess[exchange][1];
		chess[exchange][0] = tmp1;
		chess[exchange][1] = tmp2;
	}
}

bool Bot::print (Node *node, short int board_[_Table_Size][_Table_Size])
{
    if (b_or_w == Black)
        std::cout << "Black\t    Round " << round << std::endl;
    else
    	std::cout << "White\t    Round " << round << std::endl;

    //print board
    if (node)
    {
		get_board (node, 1, 0, node->depth);
    	
		for (int i = 0; i < 10; i++)
    		std::cout << "\u25A0";
    	std::cout << std::endl;
    	
        for (int y = 0; y < _Table_Size; y++)
        {
        	std::cout << "\u25A0";
        	
            for (int x = 0; x < _Table_Size; x++)
            {
            	if (board_)
            		board_[y][x] = board_t[0][y][x];
            		
				switch (board_t[0][y][x])
				{
					case Black:
						std::cout << "\u25CF";
						break;
					case White:
						std::cout << "\u25CB";
						break;
					case 0:
						std::cout << " ";
						break;
					default:
						std::cout << board_t[0][y][x];
						break;
				}
			}

        	std::cout << "\u25A0" << std::endl;			
        }
        
        for (int i = 0; i < 10; i++)
    		std::cout << "\u25A0";
    	std::cout << std::endl;
    }
    else
        std::cout << "\tSkip\n";

    return end_game ();
}

bool Bot::end_game ()
{
	int black = 0, white = 0, black_goal = 0, white_goal = 0;

	for (int i = 0; i < _Table_Size; i++)
		for (int j = 0; j < _Table_Size; j++)
		{
			if (board_t[0][i][j] == Black)
			{
				black++;
				if (j > 5)
					black_goal++;
			}

			if (board_t[0][i][j] == White)
			{
				white++;
				if (j < 2)
					white_goal++;
			}
		}

	if (!black || !white || black == black_goal || white == white_goal)
	{
		std::cout << "\tend game\n";
		if (black_goal > white_goal)
			std::cout << "\t\tblack wins\n";
            // std::cout << "0";
		if (black_goal < white_goal)
			std::cout << "\t\twhite wins\n";
            // std::cout << "1";
		if (black_goal == white_goal)
			std::cout << "\t\tties\n";
            // std::cout << "2";
		return 1;
	}
	
	return 0;
}

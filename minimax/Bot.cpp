#define _Table_Size 8
#define _Chess_Weight 100
#define Black 1
#define White -1
#define POS_INF 1000000
#define NEG_INF -1000000
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include "Bot.h"

short int up[2] = {0, -1};
short int down[2] = {0, 1};
short int left[2] = {-1, 0};
short int right[2] = {1, 0};

short int *dir[4] = {up, down, left, right};

void gotoxy (int xpos, int ypos)				/////////////////////////////////////////////////////need to clear
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle (STD_OUTPUT_HANDLE);
	scrn.X = xpos; scrn.Y = ypos;
	SetConsoleCursorPosition (hOuput,scrn);
}

Bot::Bot (short int bw, int depth)
{
	srand (time (NULL));

	round = 0;
	
    //set bot's color
    b_or_w = bw;

    //set minimax's depth
    Minimax_Depth = depth;

    root = new Node;
}

Bot::~Bot ()
{

}

void Bot::load_param ()
{

}

bool Bot::start (short int board_[_Table_Size][_Table_Size])
{
	round++;
	
	clock_t time;

	time = clock ();
	    	
    //copy table
    for (int i = 0; i < _Table_Size; i++)
        for (int j = 0; j < _Table_Size; j++)
            board[i][j] = board_[i][j];
    
    //initialize root
    root->value = NEG_INF;
    root->depth = 0;
    root->a = NEG_INF;
    root->b = POS_INF;
    root->child_count = 0;
    root->parent = NULL;
    
    //start minimax
    int solution = Max (root);
    Node *node = NULL;
    
    //search for solution
    for (int i = 0; i < root->child_count; i++)
        if (root->child[i]->value == solution)
            if (node == 0 || cal_value (node, 0) < cal_value (root->child[i], 0))
                node = root->child[i];

	bool end = print (node, board_);
		
	time = clock () - time;
	std::cout << "\n\t\t" << (double)time / CLK_TCK << " sec\n";

	return end;
}

int Bot::Max (Node *node)
{
    int v = NEG_INF, v_t;
    short int move_order[4] = {0, 1, 2, 3};
    short int jump_order[4] = {0, 1, 2, 3};
    int chess[9][2];
    int chess_count = 0;
    
    shuffle_order (move_order);
    shuffle_order (jump_order);
    
    node->child_count = 0;

    //if is end, return value
    if (node->depth == Minimax_Depth)
        return cal_value (node, 1);

    get_board (node, 1);

    for (int y = 0; y < _Table_Size; y++)
        for (int x = 0; x < _Table_Size; x++)
        	if (board_t[y][x] == b_or_w)
			{
				chess[chess_count][0] = x;
				chess[chess_count][1] = y;
				chess_count++;
			}

	shuffle_chess (chess, chess_count);

    for (int k = 0; k < chess_count; k++)
    {
    	int x = chess[k][0];
    	int y = chess[k][1];
    	
        //move up, down, left, right
        for (int i = 0; i < 4; i++)
            if (can_move (x, y, dir[move_order[i]][0], dir[move_order[i]][1]))
            {
                v_t = Mini (do_move (node, x, y, move_order[i]));
                v = (v > v_t)?v:v_t;

                if (v >= node->b)
                {
                    node->value = v;
                    return node->value;
                }
                node->a = (node->a > v)?node->a:v;

                get_board (node, 1);
            }

        //the node that try to keep jumping
        int start = node->child_count;
        Node *node_now = node;

        //position of moving chess
        int x_t = x;
        int y_t = y;

        //clear the path(-2), for dfs
        bool clear_path = 1;

        while (1)
        {
            //jump up, down, left, right
            for (int i = 0; i < 4; i++)
                if (can_jump (x_t, y_t, dir[jump_order[i]][0], dir[jump_order[i]][1]))
                {
                    v_t = Mini (do_jump (node, node_now, clear_path, x_t, y_t, jump_order[i]));
                    v = (v > v_t)?v:v_t;

                    if (v >= node->b)
                    {
                        node->value = v;
                        return node->value;
                    }
                    node->a = (node->a > v)?node->a:v;

                    get_board (node_now, clear_path);
                }

            //if no more child, then break
            if (start < node->child_count)
            {
                node_now = node->child[start++];
                x_t = node_now->x;
                y_t = node_now->y;

                clear_path = 0;

                get_board (node_now, 0);
            }
            else
                break;
        }
        
        get_board (node, 1);
    }

    //if no child, return value
    if (node->child_count == 0)
        return cal_value (node, 1);

    node->value = v;
    return node->value;
}

int Bot::Mini (Node *node)
{
    int v = POS_INF, v_t;
    short int move_order[4] = {0, 1, 2, 3};
    short int jump_order[4] = {0, 1, 2, 3};
    int chess[9][2];
    int chess_count = 0;
    
    shuffle_order (move_order);
    shuffle_order (jump_order);
    
    node->child_count = 0;

    //if is end, return value
    if (node->depth == Minimax_Depth)
        return cal_value (node, 1);

    get_board (node, 1);

    for (int y = 0; y < _Table_Size; y++)
        for (int x = 0; x < _Table_Size; x++)
        	if (board_t[y][x] == -b_or_w)
			{
				chess[chess_count][0] = x;
				chess[chess_count][1] = y;
				chess_count++;
			}

	shuffle_chess (chess, chess_count);

    for (int k = 0; k < chess_count; k++)
    {
    	int x = chess[k][0];
    	int y = chess[k][1];
    	
        //move up, down, left, right
        for (int i = 0; i < 4; i++)
            if (can_move (x, y, dir[move_order[i]][0], dir[move_order[i]][1]))
            {
                v_t = Max (do_move (node, x, y, move_order[i]));
                v = (v < v_t)?v:v_t;

                if (v <= node->a && node->depth > 1)
                {
                    node->value = v;
                    return node->value;
                }
                node->b = (node->b < v)?node->b:v;

                get_board (node, 1);
            }

        //the node that try to keep jumping
        int start = node->child_count;
        Node *node_now = node;

        //position of moving chess
        int x_t = x;
        int y_t = y;

        //clear the path(-2), for dfs
        bool clear_path = 1;

        while (1)
        {
            //jump up, down, left, right
            for (int i = 0; i < 4; i++)
                if (can_jump (x_t, y_t, dir[jump_order[i]][0], dir[jump_order[i]][1]))
                {
                    v_t = Max (do_jump (node, node_now, clear_path, x_t, y_t, jump_order[i]));
                    v = (v < v_t)?v:v_t;

                    if (v <= node->a && node->depth > 1)
                    {
                        node->value = v;
                        return node->value;
                    }
                    node->b = (node->b < v)?node->b:v;

                    get_board (node_now, clear_path);
                }

            //if no more child, then break
            if (start < node->child_count)
            {
                node_now = node->child[start++];
                x_t = node_now->x;
                y_t = node_now->y;

                clear_path = 0;

                get_board (node_now, 0);
            }
            else
                break;
        }
        
        get_board (node, 1);
    }

    //if no child, return value
    if (node->child_count == 0)
        return cal_value (node, 1);

    node->value = v;
    return node->value;
}

Node *Bot::do_move (Node *node, int x, int y, int direct)
{
    //increase vector size if needed
    if (node->child.size () < node->child_count + 1)
        node->child.resize (node->child.size () + 5);
    
    //create node if had not been created
    Node *tmp = node->child[node->child_count];
    if (tmp == NULL)
    {
        tmp = new Node;
        node->child[node->child_count] = tmp;
    }

    //set data
    tmp->depth = node->depth + 1;
    tmp->a = node->a;
    tmp->b = node->b;
    tmp->parent = node;

    //set path
    if (!tmp->path.empty ())
    	delete tmp->path.back ();
    tmp->path.clear ();

    Path *tmp_path = new Path;
    tmp_path->x = x;
    tmp_path->y = y;
    tmp_path->direct = direct;
    tmp_path->move_jump = 0;

    tmp->path.push_back (tmp_path);

    //add count
    (node->child_count)++;

    return tmp;
}

Node *Bot::do_jump (Node *parent, Node *node, bool clear_path, int x, int y, int direct)
{
    //increase vector size if needed
    if (parent->child.size () < parent->child_count + 1)
        parent->child.resize (parent->child.size () + 5);

    //create node if had not been created
    Node *tmp = parent->child[parent->child_count];
    if (tmp == NULL)
    {
        tmp = new Node;
        parent->child[parent->child_count] = tmp;
    }

    //set data
    tmp->depth = parent->depth + 1;
    tmp->a = parent->a;
    tmp->b = parent->b;
    tmp->x = x + 2 * dir[direct][0];
    tmp->y = y + 2 * dir[direct][1];
    tmp->parent = parent;

    //set path
    if (!tmp->path.empty ())
    	delete tmp->path.back ();
    tmp->path.clear ();
    
    Path *tmp_path = new Path;
    tmp_path->x = x;
    tmp_path->y = y;
    tmp_path->direct = direct;
    tmp_path->move_jump = 1;
	
    if (parent != node)
	    tmp->path = node->path;
	tmp->path.push_back (tmp_path);

    //add count
    (parent->child_count)++;

    return tmp;
}

int Bot::cal_value (Node *node, bool rewrite)
{
    int value = 0;

    get_board (node, 1);
    
    for (int y = 0; y < _Table_Size; y++)
        for (int x = 0; x < _Table_Size; x++)
        {
        	if (b_or_w == Black)
        	{
	            if (board_t[y][x] == Black)
	                value += weight[x] + _Chess_Weight - 10;
	            if (board_t[y][x] == White)
	                value += -weight[_Table_Size - x - 1] - _Chess_Weight;
			}
			else
			{
				if (board_t[y][x] == White)
	                value += weight[_Table_Size - x - 1] + _Chess_Weight - 10;
	            if (board_t[y][x] == Black)
	                value += -weight[x] - _Chess_Weight;
			}
        }

    if (rewrite)
	    node->value = value;

    return value;
}

bool Bot::can_move (int x, int y, int d_x, int d_y)
{
    if (in_bound (x + d_x, y + d_y) && !is_chess (x + d_x, y + d_y))
        return true;
    return false;
}

bool Bot::can_jump (int x, int y, int d_x, int d_y)
{
    if (in_bound (x + 2 * d_x, y + 2 * d_y) && is_chess (x + d_x, y + d_y) && board_t[y + 2 * d_y][x + 2 * d_x] == 0)
        return true;
    return false;
}

bool Bot::is_chess (int x, int y)
{
    if (board_t[y][x] == b_or_w || board_t[y][x] == -b_or_w)
        return true;
    return false;
}

bool Bot::in_bound (int x, int y)
{
    if (x >= 0 && x < _Table_Size && y >= 0 && y < _Table_Size)
        return true;
    return false;
}

void Bot::get_board (Node *node, bool clear_path)
{
    if (node->parent == NULL)
    {
    	for (int i = 0; i < _Table_Size; i++)
        	for (int j = 0; j < _Table_Size; j++)
            	board_t[i][j] = board[i][j];
        return;
	}

    //get path from root to node
    get_board (node->parent, 1);

    for (int i = 0; i < node->path.size (); i++)
    {
        int x = node->path[i]->x, y = node->path[i]->y;
        int d_x = dir[node->path[i]->direct][0], d_y = dir[node->path[i]->direct][1];
		
        if (node->path[i]->move_jump)
        {
            //jump
            board_t[y + 2 * d_y][x + 2 * d_x] = board_t[y][x];
            if (board_t[y + d_y][x + d_x] == -board_t[y][x])
                board_t[y + d_y][x + d_x] = 0;
            if (clear_path)
            	board_t[y][x] = 0;
            else
            	board_t[y][x] = -2;
        }
        else
        {
            //move
            board_t[y + d_y][x + d_x] = board_t[y][x];
            board_t[y][x] = 0;
        }
    }
}

void Bot::shuffle_order (short int order[4])
{
	for (int i = 0; i < 4; i++)
	{
		int exchange = rand () % 4;
		int tmp = order[i];
		order[i] = order[exchange];
		order[exchange] = tmp;
	}
}

void Bot::shuffle_chess (int chess[9][2], int count)
{
	for (int i = 0; i < count; i++)
	{
		int exchange = rand () % count;
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
	gotoxy (0, 0);						//////////////////////////////////////////////////////////need to clear
	
	get_board (node, 1);
	
    if (b_or_w == Black)
        std::cout << "Black\t    Round " << round << std::endl;
    else
    	std::cout << "White\t    Round " << round << std::endl;

    //print board
    if (node)
    {
    	for (int i = 0; i < 10; i++)
    		std::cout << "¡½";
    	std::cout << std::endl;
    	
        for (int y = 0; y < _Table_Size; y++)
        {
        	std::cout << "¡½";
        	
            for (int x = 0; x < _Table_Size; x++)
            {
            	if (board_)
            		board_[y][x] = board_t[y][x];
            		
	            if (board_t[y][x] == Black)
	            	std::cout << "¡³";
	            if (board_t[y][x] == White)
	            	std::cout << "¡´";
	            if (board_t[y][x] == 0)
	            	std::cout << "  ";
			}

        	std::cout << "¡½" << std::endl;			
        }
        
        for (int i = 0; i < 10; i++)
    		std::cout << "¡½";
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
			if (board_t[i][j] == Black)
			{
				black++;
				if (j > 5)
					black_goal++;
			}

			if (board_t[i][j] == White)
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
		if (black_goal < white_goal)
			std::cout << "\t\twhite wins\n";
		if (black_goal == white_goal)
			std::cout << "\t\tties\n";
		return 1;
	}
	
	return 0;
}

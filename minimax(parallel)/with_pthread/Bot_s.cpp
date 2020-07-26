#define _Table_Size 8
#define _Chess_Weight 100
#define Black 1
#define White -1
#define POS_INF 1000000
#define NEG_INF -1000000
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <time.h>
#include <sys/time.h>
#include "Bot_s.h"

short int up_s[2] = { 0, -1 };
short int down_s[2] = { 0, 1 };
short int left_s[2] = { -1, 0 };
short int right_s[2] = { 1, 0 };

short int *dir_s[4] = { up_s, down_s, left_s, right_s };

Bot_s::Bot_s(short int bw, int depth)
{
	srand(time(NULL));

	round = 0;

	//set Bot's color
	b_or_w = bw;

	//set minimax's depth
	Minimax_Depth = depth;

	root = new Node_s;
}

Bot_s::~Bot_s()
{
	delete_node(root);
}

void Bot_s::delete_node(Node_s *node)
{
	for (int i = 0; i < node->child_count; i++)
	{
		delete_node(node->child[i]);
		delete node->child[i];
	}

	node->child.clear();
	node->child.shrink_to_fit();
}

void Bot_s::load_param()
{

}

bool Bot_s::start(short int board_[_Table_Size][_Table_Size], double *total)
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

	//initialize root
	root->value = NEG_INF;
	root->depth = 0;
	root->a = NEG_INF;
	root->b = POS_INF;
	root->child_count = 0;
	root->parent = NULL;

	node_count = 0;

	//start minimax
	int solution = Max(root);
	Node_s *node = NULL;

	//search for solution
	for (int i = 0; i < root->child_count; i++)
		if (root->child[i]->value == solution)
			if (node == 0 || cal_value(node, 0) < cal_value(root->child[i], 0))
				node = root->child[i];

	bool end = print(node, board_);

    gettimeofday (&t_end, NULL);
    time = t_end.tv_sec - t_start.tv_sec + (double)(t_end.tv_usec - t_start.tv_usec) / (double)1000000;
	// std::cout << "\n\t\t" << time << " sec\n";

	*total += time;

	return end;
}

int Bot_s::Max(Node_s *node)
{
	int v = NEG_INF, v_t;
	short int move_order[4] = { 0, 1, 2, 3 };
	short int jump_order[4] = { 0, 1, 2, 3 };
	int chess[9][2];
	int chess_count = 0;

	shuffle_order(move_order);
	shuffle_order(jump_order);

	node->child_count = 0;

	//if is end, return value
	if (node->depth == Minimax_Depth)
		return cal_value(node, 1);

	get_board(node, 1, node->depth);

	for (int y = 0; y < _Table_Size; y++)
		for (int x = 0; x < _Table_Size; x++)
			if (board_t[y][x] == b_or_w)
			{
				chess[chess_count][0] = x;
				chess[chess_count][1] = y;
				chess_count++;
			}

	shuffle_chess(chess, chess_count);

	for (int k = 0; k < chess_count; k++)
	{
		int x = chess[k][0];
		int y = chess[k][1];

		//move up, down, left, right
		for (int i = 0; i < 4; i++)
			if (can_move(x, y, dir_s[move_order[i]][0], dir_s[move_order[i]][1]))
			{
				node_count++;
				v_t = Mini(do_move_jump(node, NULL, 0, x, y, move_order[i]));
				v = (v > v_t) ? v : v_t;

				if (v >= node->b)
				{
					node->value = v;
					return node->value;
				}
				node->a = (node->a > v) ? node->a : v;

				get_board(node, 1, node->depth);
			}

		//the node that try to keep jumping
		int start = node->child_count;
		Node_s *node_now = node;

		//position of moving chess
		int x_t = x;
		int y_t = y;

		//clear the path(-2), for dfs
		bool clear_path = 1;

		while (1)
		{
			//jump up, down, left, right
			for (int i = 0; i < 4; i++)
				if (can_jump(x_t, y_t, dir_s[jump_order[i]][0], dir_s[jump_order[i]][1]))
				{
					node_count++;
					v_t = Mini(do_move_jump(node, node_now, 1, x_t, y_t, jump_order[i]));
					v = (v > v_t) ? v : v_t;

					if (v >= node->b)
					{
						node->value = v;
						return node->value;
					}
					node->a = (node->a > v) ? node->a : v;

					get_board(node_now, clear_path, node_now->depth);
				}

			//if no more child, then break
			if (start < node->child_count)
			{
				node_now = node->child[start++];
				x_t = node_now->x + 2 * dir_s [node_now->direct][0];
				y_t = node_now->y + 2 * dir_s [node_now->direct][1];

				clear_path = 0;

				get_board(node_now, 0, node_now->depth);
			}
			else
				break;
		}

		get_board(node, 1, node->depth);
	}

	//if no child, return value
	if (node->child_count == 0)
		return cal_value(node, 1);

	node->value = v;
	return node->value;
}

int Bot_s::Mini(Node_s *node)
{
	int v = POS_INF, v_t;
	short int move_order[4] = { 0, 1, 2, 3 };
	short int jump_order[4] = { 0, 1, 2, 3 };
	int chess[9][2];
	int chess_count = 0;

	shuffle_order(move_order);
	shuffle_order(jump_order);

	node->child_count = 0;

	//if is end, return value
	if (node->depth == Minimax_Depth)
		return cal_value(node, 1);

	get_board(node, 1, node->depth);

	for (int y = 0; y < _Table_Size; y++)
		for (int x = 0; x < _Table_Size; x++)
			if (board_t[y][x] == -b_or_w)
			{
				chess[chess_count][0] = x;
				chess[chess_count][1] = y;
				chess_count++;
			}

	shuffle_chess(chess, chess_count);

	for (int k = 0; k < chess_count; k++)
	{
		int x = chess[k][0];
		int y = chess[k][1];

		//move up, down, left, right
		for (int i = 0; i < 4; i++)
			if (can_move(x, y, dir_s[move_order[i]][0], dir_s[move_order[i]][1]))
			{
				node_count++;
				v_t = Max(do_move_jump(node, NULL, 0, x, y, move_order[i]));
				v = (v < v_t) ? v : v_t;

				if (v <= node->a && node->depth > 1)
				{
					node->value = v;
					return node->value;
				}
				node->b = (node->b < v) ? node->b : v;

				get_board(node, 1, node->depth);
			}

		//the node that try to keep jumping
		int start = node->child_count;
		Node_s *node_now = node;

		//position of moving chess
		int x_t = x;
		int y_t = y;

		//clear the path(-2), for dfs
		bool clear_path = 1;

		while (1)
		{
			//jump up, down, left, right
			for (int i = 0; i < 4; i++)
				if (can_jump(x_t, y_t, dir_s[jump_order[i]][0], dir_s[jump_order[i]][1]))
				{
					node_count++;
					v_t = Max(do_move_jump(node, node_now, 1, x_t, y_t, jump_order[i]));
					v = (v < v_t) ? v : v_t;

					if (v <= node->a && node->depth > 1)
					{
						node->value = v;
						return node->value;
					}
					node->b = (node->b < v) ? node->b : v;

					get_board(node_now, clear_path, node_now->depth);
				}

			//if no more child, then break
			if (start < node->child_count)
			{
				node_now = node->child[start++];
				x_t = node_now->x + 2 * dir_s [node_now->direct][0];
				y_t = node_now->y + 2 * dir_s [node_now->direct][1];

				clear_path = 0;

				get_board(node_now, 0, node_now->depth);
			}
			else
				break;
		}

		get_board(node, 1, node->depth);
	}

	//if no child, return value
	if (node->child_count == 0)
		return cal_value(node, 1);

	node->value = v;
	return node->value;
}

Node_s *Bot_s::do_move_jump(Node_s *parent, Node_s *node, int move_jump, int x, int y, int direct)
{
	//increase vector size if needed
	if (parent->child.size() < parent->child_count + 1)
		parent->child.resize(parent->child.size() + 5);

	//create node if had not been created
	Node_s *tmp = parent->child[parent->child_count];
	if (tmp == NULL)
	{
		tmp = new Node_s;
		parent->child[parent->child_count] = tmp;
	}

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

int Bot_s::cal_value(Node_s *node, bool rewrite)
{
	int value = 0;

	get_board(node, 1, node->depth);

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

bool Bot_s::can_move(int x, int y, int d_x, int d_y)
{
	if (in_bound(x + d_x, y + d_y) && !is_chess(x + d_x, y + d_y))
		return true;
	return false;
}

bool Bot_s::can_jump(int x, int y, int d_x, int d_y)
{
	if (in_bound(x + 2 * d_x, y + 2 * d_y) && is_chess(x + d_x, y + d_y) && board_t[y + 2 * d_y][x + 2 * d_x] == 0)
		return true;
	return false;
}

bool Bot_s::is_chess(int x, int y)
{
	if (board_t[y][x] == b_or_w || board_t[y][x] == -b_or_w)
		return true;
	return false;
}

bool Bot_s::in_bound(int x, int y)
{
	if (x >= 0 && x < _Table_Size && y >= 0 && y < _Table_Size)
		return true;
	return false;
}

void Bot_s::get_board(Node_s *node, bool clear_path, int depth)
{
	if (node->parent == NULL)
	{
		for (int i = 0; i < _Table_Size; i++)
			for (int j = 0; j < _Table_Size; j++)
				board_t[i][j] = board[i][j];
		return;
	}

	//get path from root to node
	if (node->depth == depth)
		get_board(node->parent, clear_path, depth);
	else
		get_board(node->parent, 1, depth);

	int x = node->x, y = node->y;
	int d_x = dir_s[node->direct][0], d_y = dir_s[node->direct][1];

	if (node->move_jump)
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

void Bot_s::shuffle_order(short int order[4])
{
	for (int i = 0; i < 4; i++)
	{
		int exchange = rand() % 4;
		int tmp = order[i];
		order[i] = order[exchange];
		order[exchange] = tmp;
	}
}

void Bot_s::shuffle_chess(int chess[9][2], int count)
{
	for (int i = 0; i < count; i++)
	{
		int exchange = rand() % count;
		int tmp1 = chess[i][0];
		int tmp2 = chess[i][1];
		chess[i][0] = chess[exchange][0];
		chess[i][1] = chess[exchange][1];
		chess[exchange][0] = tmp1;
		chess[exchange][1] = tmp2;
	}
}

bool Bot_s::print(Node_s *node, short int board_[_Table_Size][_Table_Size])
{
	get_board(node, 1, node->depth);

	// if (b_or_w == Black)
	// 	std::cout << "Black\t    Round " << round << std::endl;
	// else
	// 	std::cout << "White\t    Round " << round << std::endl;

	//print board
	if (node)
	{
		// for (int i = 0; i < 10; i++)
		// 	std::cout << "\u25A0";
		// std::cout << std::endl;

		for (int y = 0; y < _Table_Size; y++)
		{
			// std::cout << "\u25A0";

			for (int x = 0; x < _Table_Size; x++)
			{
				if (board_)
					board_[y][x] = board_t[y][x];

				// switch (board_t[y][x])
				// {
				// 	case Black:
				// 		std::cout << "\u25CF";
				// 		break;
				// 	case White:
				// 		std::cout << "\u25CB";
				// 		break;
				// 	case 0:
				// 		std::cout << " ";
				// 		break;
				// 	default:
				// 		std::cout << board_t[y][x];
				// 		break;
				// }
			}

			// std::cout << "\u25A0" << std::endl;
		}

		// for (int i = 0; i < 10; i++)
		// 	std::cout << "\u25A0";
		// std::cout << std::endl;
	}
	// else
	// 	std::cout << "\tSkip\n";

	return end_game();
}

bool Bot_s::end_game()
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
		// std::cout << "\tend game\n";
		if (black_goal > white_goal)
            std::cout << "0";
			// std::cout << "\t\tblack wins\n";
		if (black_goal < white_goal)
            std::cout << "1";
			// std::cout << "\t\twhite wins\n";
		if (black_goal == white_goal)
            std::cout << "2";
			// std::cout << "\t\tties\n";
		return 1;
	}

	return 0;
}
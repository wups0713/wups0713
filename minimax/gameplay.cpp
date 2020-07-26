#define _Table_Size 8
#define Black 1
#define White -1
#include <iostream>
#include <windows.h>
#include <conio.h>
#include "gameplay.h"

short int up_[2] = {0, -1};
short int down_[2] = {0, 1};
short int left_[2] = {-1, 0};
short int right_[2] = {1, 0};

short int *dir_[4] = {up_, down_, left_, right_};

void gotoxy_(int xpos, int ypos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle (STD_OUTPUT_HANDLE);
	scrn.X = xpos; scrn.Y = ypos;
	SetConsoleCursorPosition (hOuput,scrn);
}

void cursor_clear ()
{
	HANDLE hOut = GetStdHandle (STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo (hOut,&cci);
	cci.bVisible = false;
	SetConsoleCursorInfo (hOut,&cci);
}

Player::Player (int bw)
{
	cursor_clear ();
	
	round = 0;
	
	b_or_w = bw;
	
	x_t = 0;
	y_t = 0;
	
	for (int i = 0; i < 4; i++)
	{
		move[i] = 0;
		jump[i] = 0;
	}
}

Player::~Player ()
{
	
}

bool Player::start (short int board[_Table_Size][_Table_Size])
{
	round++;
	
	//copy board
	for (int i = 0; i < _Table_Size; i++)
		for (int j = 0; j < _Table_Size; j++)
			board_t[i][j] = board[i][j];
	
	//select a chess to move, if the position is not a chess or the chess does not belong to the player then repeat
	do
	{
		select_chess ();
	}while (board_t[y_t][x_t] != b_or_w);
	
	//the first move, to avoid move after jump
	bool first = 1;

	//control the chess to move or jump
	while (1)
	{
		//detect if the chess can move or jump
		for (int i = 0; i < 4; i++)
		{
			move[i] = can_move (i);
			jump[i] = can_jump (i);
		}
		
		//if no more moves then break
		if (!first && !jump[0] && !jump[1] && !jump[2] && !jump[3])
			break;
		
		//if is move then break, if is jump then keep looping
		if (control (first))
			break;
			
		first = 0;
		
		//if end game then return 1
		if (print (board, x_t, y_t))
			return 1;
	}
	
	print (board, x_t, y_t);
	return 0;
}

void Player::select_chess ()
{
	print (0, x_t, y_t);
	
	int garbage = 0, ch1 = 0, ch2 = 0;
	while (1)
	{
		if (kbhit ())
		{
			while (kbhit ())
			{
				garbage = getch ();
				ch1 = ch2;
				ch2 = garbage;
			}
			
			if (ch1 == 224)
			{
				switch (ch2)
				{
					case 72:                    //up
						if (y_t == 0)
							y_t = 7;
						else
							y_t--;
						print (0, x_t, y_t);
						break;
					case 80:                    //down
						if (y_t == 7)
							y_t = 0;
						else
							y_t++;
						print (0, x_t, y_t);
						break;
					case 75:                    //left
						if (x_t == 0)
							x_t = 7;
						else
							x_t--;
						print (0, x_t, y_t);
						break;
					case 77:                    //right
						if (x_t == 7)
							x_t = 0;
						else
							x_t++;
						print (0, x_t, y_t);
						break;
					default:
						break;
				}
			}
			
			if (ch2 == 13)						//enter
				return;
		}
	}
}

bool Player::control (bool first)
{	
	int x_tmp = x_t;
	int y_tmp = y_t;
	int direct = 0;
	
	int garbage = 0, ch1 = 0, ch2 = 0;
	while (1)
	{
		if (kbhit ())
		{
			while (kbhit ())
			{
				garbage = getch ();
				ch1 = ch2;
				ch2 = garbage;
			}
			
			if (ch1 == 224)
			{
				switch (ch2)
				{
					case 72:                    				//up
						if ((first && move[0]) || jump[0])
						{
							if (move[0])
							{
								x_tmp = x_t;
								y_tmp = y_t - 1;
							}
							if (jump[0])
							{
								x_tmp = x_t;
								y_tmp = y_t - 2;
							}
							
							direct = 0;
							print (0, x_tmp, y_tmp);
						}
						break;
					case 80:                    				//down
						if ((first && move[1]) || jump[1])
						{
							if (move[1])
							{
								x_tmp = x_t;
								y_tmp = y_t + 1;
							}
							if (jump[1])
							{
								x_tmp = x_t;
								y_tmp = y_t + 2;
							}
							direct = 1;
							print (0, x_tmp, y_tmp);
						}
						break;
					case 75:                    				//left
						if ((first && move[2]) || jump[2])
						{
							if (move[2])
							{
								x_tmp = x_t - 1;
								y_tmp = y_t;
							}
							if (jump[2])
							{
								x_tmp = x_t - 2;
								y_tmp = y_t;
							}
							direct = 2;
							print (0, x_tmp, y_tmp);
						}
						break;
					case 77:                    				//right
						if ((first && move[3]) || jump[3])
						{
							if (move[3])
							{
								x_tmp = x_t + 1;
								y_tmp = y_t;
							}
							if (jump[3])
							{
								x_tmp = x_t + 2;
								y_tmp = y_t;
							}
							direct = 3;
							print (0, x_tmp, y_tmp);
						}
						break;
					default:
						break;
				}
			}
			
			if (ch2 == 13)										//enter
				return do_move (direct, first);
				
			if (ch2 == 32)										//space
				return 1;
		}
	}
}

bool Player::do_move (int direct, bool first)
{
	int d_x = dir_[direct][0];
	int d_y = dir_[direct][1];
	
	//move
	if (first && move[direct])
	{
		board_t[y_t + d_y][x_t + d_x] = board_t[y_t][x_t];
		board_t[y_t][x_t] = 0;
		x_t = x_t + d_x;
		y_t = y_t + d_y;
		return 1;
	}
	
	//jump
	if (jump[direct])
	{
		if (board_t[y_t + d_y][x_t + d_x] != b_or_w)
			board_t[y_t + d_y][x_t + d_x] = 0;
		board_t[y_t + d_y * 2][x_t + d_x * 2] = board_t[y_t][x_t];
		board_t[y_t][x_t] = 0;
		x_t = x_t + d_x * 2;
		y_t = y_t + d_y * 2;
		return 0;
	}
}

bool Player::can_move (int direct)
{
	int d_x = dir_[direct][0];
	int d_y = dir_[direct][1];
	
    if (in_bound (x_t + d_x, y_t + d_y) && !is_chess (x_t + d_x, y_t + d_y))
        return true;
    return false;
}

bool Player::can_jump (int direct)
{
	int d_x = dir_[direct][0];
	int d_y = dir_[direct][1];
	
    if (in_bound (x_t + d_x * 2, y_t + d_y * 2) && is_chess (x_t + d_x, y_t + d_y) && board_t[y_t + 2 * d_y][x_t + 2 * d_x] == 0)
        return true;
    return false;
}

bool Player::is_chess (int x, int y)
{
    if (board_t[y][x] == Black || board_t[y][x] == White)
        return true;
    return false;
}

bool Player::in_bound (int x, int y)
{
    if (x >= 0 && x < _Table_Size && y >= 0 && y < _Table_Size)
        return true;
    return false;
}

bool Player::print (short int board_[_Table_Size][_Table_Size], int x_tmp, int y_tmp)
{
	gotoxy_ (0, 0);

    if (b_or_w == Black)
        std::cout << "Black\t    Round " << round << std::endl;
    else
    	std::cout << "White\t    Round " << round << std::endl;

    //print board
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
        		
        	if (y == y_tmp && x == x_tmp)
        	{
        		std::cout << "¡¼";
        		continue;
			}
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

    if (end_game ())
    	return 1;
    else
    {
    	gotoxy_ (22, 6);
    	std::cout << "     ¡ô";
    	gotoxy_ (22, 7);
    	std::cout << "   ¡ö  ¡÷: control";
    	gotoxy_ (22, 8);
    	std::cout << "     ¡õ";
    	gotoxy_ (22, 9);
    	std::cout << "   enter : select";
    	gotoxy_ (22, 10);
		std::cout << "   space : stop moving";
    	return 0;
	}
}

bool Player::end_game ()
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

#define _Minimax_Depth 4
#define _Table_Size 8
#define Black 1
#define White -1

#include <iostream>
#include <iomanip>
#include <ctime>
#include <windows.h>
#include "Bot.h"

int main (void)
{
	Bot bot (Black, _Minimax_Depth);
    Bot bot1 (White, _Minimax_Depth);
    
	short int board[_Table_Size][_Table_Size] = 
	{
		{  1,  0,  0,  0,  0,  0,  0,  0},
		{  0,  1,  0,  0,  0,  0,  0, -1},
		{  1,  0,  1,  0,  0,  0, -1,  0},
		{  0,  1,  0,  0,  0, -1,  0, -1},
		{  1,  0,  1,  0,  0,  0, -1,  0},
		{  0,  1,  0,  0,  0, -1,  0, -1},
		{  1,  0,  0,  0,  0,  0, -1,  0},
		{  0,  0,  0,  0,  0,  0,  0, -1}
	};
    
    //start playing
    while (1)
    {
    	//black
    	if (bot.start(board))
			break;
    	
    	//white
		if (bot1.start (board))
    		break;
	}
	system ("pause");
}

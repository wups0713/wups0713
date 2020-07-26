#define _Minimax_Depth 5
#define _Table_Size 8
#define Black 1
#define White -1

#include <iostream>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include "Bot.h"
#include "Bot_s.h"

int main (void)
{
    //start playing
	while (1)
	{
		Bot bot (Black, _Minimax_Depth);
		Bot_s bot1 (White, _Minimax_Depth);
		double b_total = 0.0, w_total = 0.0;
		long long int b_node_count = 0, w_node_count = 0;
		int n;

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
    
		for (n = 1; n < 201; n++)
		{
    		//black
    		if (bot.start(board, &b_total))
				break;
			// b_node_count += bot.node_count;
			
    		//white
			if (bot1.start (board, &w_total))
    			break;
			// w_node_count += bot1.node_count [0][0];

			if (n == 200)
				std::cout << "2";
			//getc (stdin);
		}

		// std::cout << "\nBlack avg: " << b_total / n << " sec\nwhite avg: " << w_total / n << " sec\n\t\t" << b_total / w_total << std::endl;
		// std::cout << "\nBlack nodes: " << b_node_count << "\nWhite nodes: " << w_node_count << std::endl;
		std::cout << "\t" << b_total / n << std::endl;
		break;
	}
}

#include <iostream>
#include <windows.h>
#include <iomanip>
#include <conio.h>
#include "block.h"

using namespace std;

int Block::home_page_control ()
{
	int ch = 0, T = 0;
	while (1)
	{
		T++;
		while (kbhit ())
			ch = getch ();
		switch (ch)
		{
			case 72:        //up
				return -1;
			case 80:        //down
				return 1;
			case 75:        //left
				return -1;
			case 77:        //right
				return 1;
			case 13:        //enter
				return 0;
			default:
				break;
		}
		gotoxy (arrow_x, arrow_y);
		if ((T / 6000) % 2)
			cout << "  ";
		else
			cout << "=>";
		if (T > 100000)
			T = 0;
	}
}

bool Block::home_page (int page)
{
	int page0 = 0, page3 = 0;
	int select;
	cout << "                                                    \n"
		 << "                                                    \n"
		 << "  ▇▇▇▇▇                          ▇            \n"
		 << "      ▇      ▇▇      ▇    ▇  ▇        ▇▇▇  \n"
		 << "      ▇    ▇    ▇  ▇▇▇  ▇▇    ▇  ▇        \n"
		 << "      ▇    ▇▇▇▇    ▇    ▇      ▇    ▇▇    \n"
		 << "      ▇    ▇          ▇    ▇      ▇        ▇  \n"
		 << "      ▇      ▇▇▇    ▇    ▇      ▇  ▇▇▇    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "====================================================\n"
		 << "====================================================\n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "                                                    \n"
		 << "====================================================\n"
		 << "====================================================\n"
		 << "                                                    \n";
	while (1)
	{
		switch (page)
		{
			case 0:
				{
					page0 = 0;
					arrow_x = 22;
					arrow_y = 15;
					gotoxy (0, 12);
					cout << "                                                    \n"
						 << "                                                    \n"
						 << "                                                    \n"
						 << "                          開始遊戲                  \n"
						 << "                                                    \n"
						 << "                          遊戲說明                  \n"
						 << "                                                    \n"
						 << "                          設定難度                  \n"
						 << "                                                    \n"
						 << "                          離開遊戲                  \n"
						 << "                                                    \n"
						 << "                                                    \n"
						 << "                                                    \n";
					while (select = home_page_control ())
					{
						page0 = ((page0 + select < 0) || (page0 + select > 3))?page0:(page0 + select);
						for (int i = 0; i < 4; i++)
						{
							gotoxy (22, 15 + 2 * i);
							cout << "   ";
						}
						arrow_y = 15 + 2 * page0;
					}
					switch (page0)
					{
						case 0:
							return 1;
						case 1:
							page = 1;
							break;
						case 2:
							page = 2;
							break;
						case 3:
							return 0;
					}
					break;
				}
			case 1:
				{
					arrow_x = 22;
					arrow_y = 21;
					gotoxy (0, 12);
					cout << "                                                    \n"
						 << "                                                    \n"
						 << "                                                    \n"
						 << "             ← →:控制方向                         \n"
						 << "                                                    \n"
						 << "                ↑:旋轉方塊                         \n"
						 << "                                                    \n"
						 << "                ↓:加速下降                         \n"
						 << "                                                    \n"
						 << "                          返回                      \n"
						 << "                                                    \n"
						 << "                                                    \n"
						 << "                                                    \n";
					while (home_page_control ());
					page = 0;
					break;
				}
			case 2:
				{
					arrow_x = 22;
					arrow_y = 21;
					gotoxy (0, 12);
					cout << "                                                    \n"
						 << "                                                    \n"
						 << "                                                    \n"
						 << "                         ↑                         \n"
						 << "                                                    \n"
						 << "                         " << level << "                       \n"
						 << "                                                    \n"
						 << "                         ↓                         \n"
						 << "                                                    \n"
						 << "                          返回                      \n"
						 << "                                                    \n"
						 << "                                                    \n"
						 << "                                                    \n";
					while (select = home_page_control ())
					{
						level = ((level - select < 1) || (level - select > 5))?level:(level - select);
						gotoxy (25, 17);
						cout << level;
					}
					page = 0;
					break;
				}
			case 3:
				{
					highest_score = (score > highest_score)?score:highest_score;
					arrow_x = 22;
					arrow_y = 19;
					gotoxy (0, 12);
					cout << "                                                    \n"
						 << "                                                    \n"
						 << "                                                    \n"
						 << "     highest score:          your score:            \n"
						 << "                                                    \n"
						 << "           " << setw (8) << highest_score << "                " << setw (8) << score << "         \n"
						 << "                                                    \n"
						 << "                          繼續遊戲                  \n"
						 << "                                                    \n"
						 << "                          離開遊戲                  \n"
						 << "                                                    \n"
						 << "                                                    \n"
						 << "                                                    \n";
					while (select = home_page_control ())
					{
						page3 = ((page3 + select < 0) || (page3 + select > 2))?page3:(page3 + select);
						for (int i = 0; i < 2; i++)
						{
							gotoxy (22, 19 + 2 * i);
							cout << "   ";
						}
						arrow_y = 19 + 2 * page3;
					}
					switch (page3)
					{
						case 0:
							return 1;
						case 1:
							return 0;
					}
					break;
				}
			default:
				break;
		}
	}
}

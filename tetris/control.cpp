#include <iostream>
#include <conio.h>                          //getch ()
#include "block.h"

using namespace std;

bool Block::control ()
{
	int garbage = 0, ch1 = 0, ch2 = 0;
	if (kbhit ())
	{
		while (kbhit ())
		{
			garbage = getch ();
			ch1 = ch2;
			ch2 = garbage;
		}
		if (ch1 == 224)                     //鍵盤功能按鍵輸入時會回傳兩個特殊值 ch1 方向鍵 ch2 上下左右
		{
			switch (ch2)                    //判斷上下左右
			{
				case 72:                    //0x48
					turn ();
					return 1;
				case 80:                    //0x50
					return move_down ();
				case 75:                    //0x4D
					move_left ();
					return 1;
				case 77:                    //0x4B
					move_right ();
					return 1;
				default:
					return 1;
			}
		}
	}
	return 1;
}

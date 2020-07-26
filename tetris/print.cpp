#include <iostream>
#include <windows.h>
#include <iomanip>
#include "block.h"

using namespace std;

Block::gotoxy(int xpos, int ypos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = xpos; scrn.Y = ypos;
	SetConsoleCursorPosition(hOuput,scrn);
}

void reset_color (HANDLE hOut)
{
	SetConsoleTextAttribute (hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void color (HANDLE hOut,int color)
{
	switch (color)
	{
		case -1:                                                                                        //wall
			SetConsoleTextAttribute (hOut, 119);                                                        //gray
			cout << "¢h";
			break;
		case 0:                                                                                         //blank
			SetConsoleTextAttribute (hOut, 0);                                                          //black
			cout << "¢h";
			break;
		case 1:                                                                                         //T
			SetConsoleTextAttribute (hOut, 68);														    //red
			cout << "¢h";
			break;
		case 2:                                                                                         //L
			SetConsoleTextAttribute (hOut, 17);                                             			//blue
			cout << "¢h";
			break;
		case 3:                                                                                         //opposite_L
			SetConsoleTextAttribute (hOut, 34);                                            				//green
			cout << "¢h";
			break;
		case 4:                                                                                         //O
			SetConsoleTextAttribute (hOut, 187);			                                            //bright blue
			cout << "¢h";
			break;
		case 5:                                                                                         //I
			SetConsoleTextAttribute (hOut, 238);                       			                      	//yellow
			cout << "¢h";
			break;
		case 6:                                                                                         //N
			SetConsoleTextAttribute (hOut, 170);                                             			//bright green
			cout << "¢h";
			break;
		case 7:                                                                                         //opposite_N
			SetConsoleTextAttribute (hOut, 204);                                             			//bright red
			cout << "¢h";
			break;
		default:
			break;
	}
}

Block::print ()
{
//	for (int i = 0; i < length; i++)
//	{
//		for (int j = 0; j < width; j++)
//			cout << (((j - x > -1) && (j - x < 4) && (i - y > -1) && (i - y < 4))?(structure [i][j] + now [i - y][j - x]):(structure [i][j]));
//		cout << endl;
//	}
//	getchar ();
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	for (int i = 0; i < length; i++)
	{
		gotoxy (2, 1 + i);
		for (int j = 0; j < width; j++)
		{
			int moving = 0;
			if ((j - x > -1) && (j - x < 4) && (i - y > -1) && (i - y < 4))
				moving = now [i - y][j - x];
			color (hOut, structure [i][j] + moving);
		}
		cout << endl;
	}
	gotoxy (36, 3);
	reset_color (hOut);
	cout << "    Next";
	for(int i = 0; i < 6; i++)
	{
		gotoxy (36, 4 + i);
			for (int j = 0; j < 6; j++)
			if (i == 0 || i == 5 || j == 0 || j == 5)
				color (hOut, -1);
			else
				color (hOut, Next [i - 1][j - 1]);
		cout << endl;
	}
	gotoxy (36, 14);
	reset_color (hOut);
	cout << "    Time";
	gotoxy (32, 15);
	now_time = time (NULL);
	cout << setw (12) << now_time - start_time;
	gotoxy (36, 17);
	reset_color (hOut);
	cout << "   Level";
	gotoxy (34, 18);
	cout << setw (10) << level;
	gotoxy (36, 20);
	reset_color (hOut);
	cout << "   Score";
	gotoxy (34, 21);
	cout << setw (10) << score;
	gotoxy (0, 0);
}

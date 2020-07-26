#include <iostream>
#include <windows.h>
#include "block.h"

using namespace std;

void cursor_clear ()                                            //隱藏游標
{                                                               //
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);              //**
	CONSOLE_CURSOR_INFO cci;                                    //**
	GetConsoleCursorInfo(hOut,&cci);                            //**
	cci.bVisible=false;                                         //**
	SetConsoleCursorInfo(hOut,&cci);                            //**
}                                                               //

int main (void)
{
	system("mode con: cols=52 lines=28");                       //改變視窗大小
	cursor_clear ();                                            //隱藏光標
	int record = 0;		                  	 	                //最高分
	while (1)                                                   //重複執行程式
	{
		int T = 0;                                              //執行次數 用以控制方塊下降速度
		int game_over = 0;                                      //結束遊戲
		Block block;                                            //創建class
		block.set_highest_score (record);                       //設定最高分
		block.gotoxy (0, 0);                                    //移動光標(textbox)位置
		if (!block.home_page (0))                               //呼叫home_page 進入home_page if (選擇離開遊戲)
			break;                                           	//then 結束程式
		system ("cls");                                         //清除home_page畫面
		block.create_start ();                                  //創建移動方塊 & 下個方塊
		block.get_time ();                                      //取得遊戲開始時間
		block.print ();                                         //輸出遊戲畫面
		while (!game_over)                                      //主迴圈
		{
			if (!block.control ())                              //呼叫control 以移動方塊 if (方塊被放置)
			{
				if (!block.create ())                           //then 呼叫create 以放置新移動方塊 if (無法放置新方塊)
					game_over = 1;                              //then 離開主迴圈
				continue;
			}
			if (!(T % (11 - 2 * block.get_level ())) && !block.move_down ())    //if (呼叫get_level 計算此次是否該下降            {&&有執行順序}
			{                                                                   //	  如果該下降則呼叫move_down 以下降方塊 如果方塊被放置)
				if (!block.create ())                           //then 呼叫create 以放置新移動方塊 if (無法放置新方塊)
					game_over = 1;                              //then 離開主迴圈
				continue;
			}                                                   //兩個if不能合併 需分開執行
			block.print ();                                     //輸出遊戲畫面
			Sleep (50);                                         //暫停 製造兩次移動方塊間的空隙
			T ++;
			if (T > 100000)                                     //避免overflow
				T = 0;
		}
		if (!block.home_page (3))                               //if (選擇離開遊戲)
			break;                                           	//then 結束程式
		record = block.get_highest_score ();                    //儲存最高分
	}
}

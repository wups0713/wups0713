#ifndef _block
#define _block
#include <iostream>
#include <time.h>

class Block
{
	public:
		Block ();                                                   //constructor
		~Block ();                                                  //destructor
		bool home_page (int);                                       //執行home_page
		int home_page_control ();                                   //home_page 控制
		bool create ();                                             //創建新方塊 {遊戲中}
		create_start ();                                            //創建新方塊 {開始遊戲時}
		bool move_down ();                                          //下降
		move_left ();                                               //往左
		move_right ();                                              //往右
		turn ();                                                    //旋轉
		print ();                                                   //輸出遊戲畫面
		bool control ();                                            //遊戲控制
		set ();                                                     //放置方塊
		bool check ();                                              //檢查方塊是否重疊
		clean ();                                                   //消除
		get_time ();                                                //取得遊戲開始時間
		int get_level ();                                           //取得遊戲難度
		gotoxy (int, int);                                          //控制光標(textbox)位置
		int get_highest_score ();                                   //取得最高分數
		set_highest_score (int);                                    //設定最高分數
	private:
		const static int width = (12 + 2), length = (25 + 2);       //範圍
		int arrow_x, arrow_y;                                       //home_page箭頭位置
		int x = 0, y = 0;                                           //移動方塊位置
		time_t start_time, now_time;                                //遊戲開始時間, 現在時間    {用以計算遊戲時數}
		int score = 0, highest_score = 0, level = 1;                //分數, 最高分數, 遊戲難度
		int type = 0, direction = 0;                                //移動方塊類型, 移動方塊方向
		int next_type = 0, next_direction = 0;                      //下個方塊類型, 下個方塊方向
		int structure [length][width];                              //主結構
		int temp [4][4] = {{0, 0, 0, 0},                            //暫存結構	{for turn}
						   {0, 0, 0, 0},
						   {0, 0, 0, 0},
						   {0, 0, 0, 0}};
		int now [4][4] = {{0, 0, 0, 0},                             //現在移動方塊
						  {0, 0, 0, 0},
						  {0, 0, 0, 0},
						  {0, 0, 0, 0}};
		int Next [4][4] = {{0, 0, 0, 0},                            //下個方塊
						   {0, 0, 0, 0},
						   {0, 0, 0, 0},
						   {0, 0, 0, 0}};
		int T1 [4][4] = {{0, 0, 0, 0},                              //T1
						 {1, 1, 1, 0},
						 {0, 1, 0, 0},
						 {0, 0, 0, 0}},
			T2 [4][4] = {{0, 1, 0, 0},                              //T2
						 {0, 1, 1, 0},
						 {0, 1, 0, 0},
						 {0, 0, 0, 0}},
			T3 [4][4] = {{0, 1, 0, 0},                              //T3
						 {1, 1, 1, 0},
						 {0, 0, 0, 0},
						 {0, 0, 0, 0}},
			T4 [4][4] = {{0, 1, 0, 0},                              //T4
						 {1, 1, 0, 0},
						 {0, 1, 0, 0},
						 {0, 0, 0, 0}};
		int L1 [4][4] = {{0, 2, 0, 0},                              //L1
						 {0, 2, 0, 0},
						 {0, 2, 2, 0},
						 {0, 0, 0, 0}},
			L2 [4][4] = {{0, 0, 2, 0},                              //L2
						 {2, 2, 2, 0},
						 {0, 0, 0, 0},
						 {0, 0, 0, 0}},
			L3 [4][4] = {{2, 2, 0, 0},                              //L3
						 {0, 2, 0, 0},
						 {0, 2, 0, 0},
						 {0, 0, 0, 0}},
			L4 [4][4] = {{0, 0, 0, 0},                              //L4
						 {2, 2, 2, 0},
						 {2, 0, 0, 0},
						 {0, 0, 0, 0}};
		int OL1 [4][4] = {{0, 3, 0, 0},                             //opposite L1
						  {0, 3, 0, 0},
						  {3, 3, 0, 0},
						  {0, 0, 0, 0}},
			OL2 [4][4] = {{0, 0, 0, 0},                             //opposite L2
						  {3, 3, 3, 0},
						  {0, 0, 3, 0},
						  {0, 0, 0, 0}},
			OL3 [4][4] = {{0, 3, 3, 0},                             //opposite L3
						  {0, 3, 0, 0},
						  {0, 3, 0, 0},
						  {0, 0, 0, 0}},
			OL4 [4][4] = {{3, 0, 0, 0},                             //opposite L4
						  {3, 3, 3, 0},
						  {0, 0, 0, 0},
						  {0, 0, 0, 0}};
		int O [4][4] = {{0, 0, 0, 0},                               //O
						{0, 4, 4, 0},
						{0, 4, 4, 0},
						{0, 0, 0, 0}};
		int I1 [4][4] = {{0, 5, 0, 0},                              //I1
						 {0, 5, 0, 0},
						 {0, 5, 0, 0},
						 {0, 5, 0, 0}},
			I2 [4][4] = {{0, 0, 0, 0},                              //I2
						 {5, 5, 5, 5},
						 {0, 0, 0, 0},
						 {0, 0, 0, 0}};
		int N1 [4][4] = {{0, 0, 6, 0},                              //N1
						 {0, 6, 6, 0},
						 {0, 6, 0, 0},
						 {0, 0, 0, 0}},
			N2 [4][4] = {{0, 0, 0, 0},                              //N2
						 {6, 6, 0, 0},
						 {0, 6, 6, 0},
						 {0, 0, 0, 0}};
		int ON1 [4][4] = {{0, 7, 0, 0},                             //opposite N1
						  {0, 7, 7, 0},
						  {0, 0, 7, 0},
						  {0, 0, 0, 0}},
			ON2 [4][4] = {{0, 0, 0, 0},                             //opposite N2
						  {0, 7, 7, 0},
						  {7, 7, 0, 0},
						  {0, 0, 0, 0}};
		int *ptr [28] = {(int *)T1, (int *)T2, (int *)T3, (int *)T4,            //各結構指標    {為了方便隨機}
						 (int *)L1, (int *)L2, (int *)L3, (int *)L4,
						 (int *)OL1, (int *)OL2, (int *)OL3, (int *)OL4,
						 (int *)O, (int *)O, (int *)O, (int *)O,
						 (int *)I1, (int *)I2, (int *)I1, (int *)I2,
						 (int *)N1, (int *)N2, (int *)N1, (int *)N2,
						 (int *)ON1, (int *)ON2, (int *)ON1, (int *)ON2};
};

#endif

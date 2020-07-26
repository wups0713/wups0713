#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <iomanip>
#include "block.h"

using namespace std;

Block::Block ()                                             //constructor
{
	for (int i = 0; i < length; i++)                        //創建牆壁
	{
		if (i == 0 || i == (length - 1))
			for (int j = 0; j < width; j++)
				structure [i][j] = -1;
		else
			for (int j = 0; j < width; j++)
				if (j == 0 || j == (width - 1))
					structure [i][j] = -1;
				else
					structure [i][j] = 0;
	}
}

Block::~Block ()                                            //destructor
{
}

Block::create_start ()                                      //創建新方塊 {開始遊戲時}
{
	x = 5;	y = 2;                                          //設定移動方塊創建位置
	srand ((unsigned)time (NULL));                          //利用時間生成隨機種子碼
	type = rand () % 7;                                     //隨機選取一種類型
	direction = rand () % 4;                                //隨機選取一個方向
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			now [i][j] = ptr [type * 4 + direction][i * 4 + j];     //創建現在移動方塊
	next_type = rand () % 7;                                //隨機選取一種類型
	next_direction = rand () % 4;                           //隨機選取一個方向
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Next [i][j] = ptr [next_type * 4 + next_direction][i * 4 + j];      //創建下個移動方塊
}

bool Block::create ()                                       //創建新方塊 {遊戲中}
{
	x = 5;	y = 2;                                          //設定移動方塊創建位置
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			now [i][j] = Next [i][j];                       //將下個移動方塊變成現在方塊
	type = next_type;                                       
	direction = next_direction;                             //複製方塊資訊
	srand ((unsigned)time (NULL));
	next_type = rand () % 7;                                //隨機選取一種類型
	next_direction = rand () % 4;                           //隨機選取一個方向
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Next [i][j] = ptr [next_type * 4 + next_direction][i * 4 + j];      //創建下個移動方塊
	return check ();                                        //執行check 檢查方塊是否可以創建
}

Block::turn ()                                              //旋轉
{
	int temp_direction = direction, next_direction = direction + 1;     //備份現在方向  旋轉
	if (direction == 3)
		next_direction = 0;                                             //避免overflow
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			temp [i][j] = now [i][j];                                   //備份現在移動方塊
			now [i][j] = ptr [type * 4 + next_direction][i * 4 + j];    //旋轉方塊
		}
	direction = next_direction;                                         //改變方向
	if (!check ())                                                      //if (方塊重疊)
	{
		direction = temp_direction;                                     //then 回復方向
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				now [i][j]= temp [i][j];                                //then 回復方塊
	}
}

bool Block::move_down ()                //下降
{
	y++;                                //垂直位置加1
	if (!check ())                      //if (方塊重疊)
	{
		y--;                            //then 回復方塊位置
		set ();                         //then 放置方塊
		return 0;                       //then 成功放置方塊
	}
	return 1;                           //沒有放置方塊
}

Block::move_right ()                    //往右
{
	x++;                                //水平位置加1
	if (!check ())                      //if (方塊重疊)
		x--;                            //then 回復方塊位置
}

Block::move_left ()                     //往左
{
	if (x > 0)                          //避免overflow  {往右不會overflow}
	{
		x--;                            //then 水平位置減1
		if (!check ())                  //if (方塊重疊)
			x++;                        //then 回復方塊位置
	}
}

bool Block::check ()                    				//檢查方塊是否重疊
{
	int moving = 0, table = 0;                          //現在移動方塊, 檯面上方塊
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			moving = now [i][j];
			if ((x + j > 13) || (y + i > 26))           //if (overflow)
				table = 0;                              //then table預設為0
			else
				table = structure [y + i][x + j];
			if (moving && table)                        //if (方塊重疊)
				return 0;                               //then 回傳錯誤
		}
	return 1;   										//回傳正確
}

Block::set ()                                                                   //放置方塊
{
	for (int i = 0; i < length; i++)
		for (int j = 0; j < width; j++)
			if ((j - x > -1) && (j - x < 4) && (i - y > -1) && (i - y < 4))
				structure [i][j] = structure [i][j] + now [i - y][j - x];       //將現在移動方塊合併到檯面
	clean ();                                                                   //判斷是否需消除
}

Block::clean ()                                 //消除
{
	int add_score = 0;                          //消除數量
	for (int i = 0; i < 4; i++)
	{
		int test = 1;                           //判斷是否已滿
		if (y + i < 26)
		{
			for (int j = 1; (j < 13) && (test > 0); j++)
				test *= structure [y + i][j];               //判斷該行是否有空格
			if (test)                                       //if (該行已滿)
			{
				int movedown = y + i;                       //該消除的行
				while (movedown > 1)                        //將以上每一行往下移動
				{
					memcpy (structure [movedown], structure [movedown - 1], sizeof (structure [0]));
					movedown--;
				}
				add_score++;
			}
		}
	}
	switch (add_score)                          //計算加分
	{
		case 1:                                 //消除一行
			score += 100;
			break;
		case 2:                                 //消除兩行
			score += 300;                       
			break;
		case 3:                                 //消除三行
			score += 600;                       
			break;
		case 4:                                 //消除四行
			score += 1000;                      
			break;
		default:
			break;
	}
	level = (level < 5 && level < (score / 500 + 1))?(score / 500 + 1):level;       //計算難度 每500分難度增加
}

Block::get_time ()                  //取得遊戲開始時間
{
	start_time = time (NULL);
}

int Block::get_level ()             //取得遊戲難度
{
	return level;
}

int Block::get_highest_score ()     //取得最高分數
{
	return highest_score;
}

Block::set_highest_score (int record)   //設定最高分數
{
	highest_score = record;
}

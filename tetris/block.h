#ifndef _block
#define _block
#include <iostream>
#include <time.h>

class Block
{
	public:
		Block ();                                                   //constructor
		~Block ();                                                  //destructor
		bool home_page (int);                                       //����home_page
		int home_page_control ();                                   //home_page ����
		bool create ();                                             //�Ыطs��� {�C����}
		create_start ();                                            //�Ыطs��� {�}�l�C����}
		bool move_down ();                                          //�U��
		move_left ();                                               //����
		move_right ();                                              //���k
		turn ();                                                    //����
		print ();                                                   //��X�C���e��
		bool control ();                                            //�C������
		set ();                                                     //��m���
		bool check ();                                              //�ˬd����O�_���|
		clean ();                                                   //����
		get_time ();                                                //���o�C���}�l�ɶ�
		int get_level ();                                           //���o�C������
		gotoxy (int, int);                                          //�������(textbox)��m
		int get_highest_score ();                                   //���o�̰�����
		set_highest_score (int);                                    //�]�w�̰�����
	private:
		const static int width = (12 + 2), length = (25 + 2);       //�d��
		int arrow_x, arrow_y;                                       //home_page�b�Y��m
		int x = 0, y = 0;                                           //���ʤ����m
		time_t start_time, now_time;                                //�C���}�l�ɶ�, �{�b�ɶ�    {�ΥH�p��C���ɼ�}
		int score = 0, highest_score = 0, level = 1;                //����, �̰�����, �C������
		int type = 0, direction = 0;                                //���ʤ������, ���ʤ����V
		int next_type = 0, next_direction = 0;                      //�U�Ӥ������, �U�Ӥ����V
		int structure [length][width];                              //�D���c
		int temp [4][4] = {{0, 0, 0, 0},                            //�Ȧs���c	{for turn}
						   {0, 0, 0, 0},
						   {0, 0, 0, 0},
						   {0, 0, 0, 0}};
		int now [4][4] = {{0, 0, 0, 0},                             //�{�b���ʤ��
						  {0, 0, 0, 0},
						  {0, 0, 0, 0},
						  {0, 0, 0, 0}};
		int Next [4][4] = {{0, 0, 0, 0},                            //�U�Ӥ��
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
		int *ptr [28] = {(int *)T1, (int *)T2, (int *)T3, (int *)T4,            //�U���c����    {���F��K�H��}
						 (int *)L1, (int *)L2, (int *)L3, (int *)L4,
						 (int *)OL1, (int *)OL2, (int *)OL3, (int *)OL4,
						 (int *)O, (int *)O, (int *)O, (int *)O,
						 (int *)I1, (int *)I2, (int *)I1, (int *)I2,
						 (int *)N1, (int *)N2, (int *)N1, (int *)N2,
						 (int *)ON1, (int *)ON2, (int *)ON1, (int *)ON2};
};

#endif

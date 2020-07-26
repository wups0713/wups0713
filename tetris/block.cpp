#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <iomanip>
#include "block.h"

using namespace std;

Block::Block ()                                             //constructor
{
	for (int i = 0; i < length; i++)                        //�Ы����
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

Block::create_start ()                                      //�Ыطs��� {�}�l�C����}
{
	x = 5;	y = 2;                                          //�]�w���ʤ���Ыئ�m
	srand ((unsigned)time (NULL));                          //�Q�ήɶ��ͦ��H���ؤl�X
	type = rand () % 7;                                     //�H������@������
	direction = rand () % 4;                                //�H������@�Ӥ�V
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			now [i][j] = ptr [type * 4 + direction][i * 4 + j];     //�Ыز{�b���ʤ��
	next_type = rand () % 7;                                //�H������@������
	next_direction = rand () % 4;                           //�H������@�Ӥ�V
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Next [i][j] = ptr [next_type * 4 + next_direction][i * 4 + j];      //�ЫؤU�Ӳ��ʤ��
}

bool Block::create ()                                       //�Ыطs��� {�C����}
{
	x = 5;	y = 2;                                          //�]�w���ʤ���Ыئ�m
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			now [i][j] = Next [i][j];                       //�N�U�Ӳ��ʤ���ܦ��{�b���
	type = next_type;                                       
	direction = next_direction;                             //�ƻs�����T
	srand ((unsigned)time (NULL));
	next_type = rand () % 7;                                //�H������@������
	next_direction = rand () % 4;                           //�H������@�Ӥ�V
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Next [i][j] = ptr [next_type * 4 + next_direction][i * 4 + j];      //�ЫؤU�Ӳ��ʤ��
	return check ();                                        //����check �ˬd����O�_�i�H�Ы�
}

Block::turn ()                                              //����
{
	int temp_direction = direction, next_direction = direction + 1;     //�ƥ��{�b��V  ����
	if (direction == 3)
		next_direction = 0;                                             //�קKoverflow
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			temp [i][j] = now [i][j];                                   //�ƥ��{�b���ʤ��
			now [i][j] = ptr [type * 4 + next_direction][i * 4 + j];    //������
		}
	direction = next_direction;                                         //���ܤ�V
	if (!check ())                                                      //if (������|)
	{
		direction = temp_direction;                                     //then �^�_��V
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				now [i][j]= temp [i][j];                                //then �^�_���
	}
}

bool Block::move_down ()                //�U��
{
	y++;                                //������m�[1
	if (!check ())                      //if (������|)
	{
		y--;                            //then �^�_�����m
		set ();                         //then ��m���
		return 0;                       //then ���\��m���
	}
	return 1;                           //�S����m���
}

Block::move_right ()                    //���k
{
	x++;                                //������m�[1
	if (!check ())                      //if (������|)
		x--;                            //then �^�_�����m
}

Block::move_left ()                     //����
{
	if (x > 0)                          //�קKoverflow  {���k���|overflow}
	{
		x--;                            //then ������m��1
		if (!check ())                  //if (������|)
			x++;                        //then �^�_�����m
	}
}

bool Block::check ()                    				//�ˬd����O�_���|
{
	int moving = 0, table = 0;                          //�{�b���ʤ��, �i���W���
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			moving = now [i][j];
			if ((x + j > 13) || (y + i > 26))           //if (overflow)
				table = 0;                              //then table�w�]��0
			else
				table = structure [y + i][x + j];
			if (moving && table)                        //if (������|)
				return 0;                               //then �^�ǿ��~
		}
	return 1;   										//�^�ǥ��T
}

Block::set ()                                                                   //��m���
{
	for (int i = 0; i < length; i++)
		for (int j = 0; j < width; j++)
			if ((j - x > -1) && (j - x < 4) && (i - y > -1) && (i - y < 4))
				structure [i][j] = structure [i][j] + now [i - y][j - x];       //�N�{�b���ʤ���X�֨��i��
	clean ();                                                                   //�P�_�O�_�ݮ���
}

Block::clean ()                                 //����
{
	int add_score = 0;                          //�����ƶq
	for (int i = 0; i < 4; i++)
	{
		int test = 1;                           //�P�_�O�_�w��
		if (y + i < 26)
		{
			for (int j = 1; (j < 13) && (test > 0); j++)
				test *= structure [y + i][j];               //�P�_�Ӧ�O�_���Ů�
			if (test)                                       //if (�Ӧ�w��)
			{
				int movedown = y + i;                       //�Ӯ�������
				while (movedown > 1)                        //�N�H�W�C�@�橹�U����
				{
					memcpy (structure [movedown], structure [movedown - 1], sizeof (structure [0]));
					movedown--;
				}
				add_score++;
			}
		}
	}
	switch (add_score)                          //�p��[��
	{
		case 1:                                 //�����@��
			score += 100;
			break;
		case 2:                                 //�������
			score += 300;                       
			break;
		case 3:                                 //�����T��
			score += 600;                       
			break;
		case 4:                                 //�����|��
			score += 1000;                      
			break;
		default:
			break;
	}
	level = (level < 5 && level < (score / 500 + 1))?(score / 500 + 1):level;       //�p������ �C500�����׼W�[
}

Block::get_time ()                  //���o�C���}�l�ɶ�
{
	start_time = time (NULL);
}

int Block::get_level ()             //���o�C������
{
	return level;
}

int Block::get_highest_score ()     //���o�̰�����
{
	return highest_score;
}

Block::set_highest_score (int record)   //�]�w�̰�����
{
	highest_score = record;
}

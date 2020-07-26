#include <iostream>
#include <windows.h>
#include "block.h"

using namespace std;

void cursor_clear ()                                            //���ô��
{                                                               //
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);              //**
	CONSOLE_CURSOR_INFO cci;                                    //**
	GetConsoleCursorInfo(hOut,&cci);                            //**
	cci.bVisible=false;                                         //**
	SetConsoleCursorInfo(hOut,&cci);                            //**
}                                                               //

int main (void)
{
	system("mode con: cols=52 lines=28");                       //���ܵ����j�p
	cursor_clear ();                                            //���å���
	int record = 0;		                  	 	                //�̰���
	while (1)                                                   //���ư���{��
	{
		int T = 0;                                              //���榸�� �ΥH�������U���t��
		int game_over = 0;                                      //�����C��
		Block block;                                            //�Ы�class
		block.set_highest_score (record);                       //�]�w�̰���
		block.gotoxy (0, 0);                                    //���ʥ���(textbox)��m
		if (!block.home_page (0))                               //�I�shome_page �i�Jhome_page if (������}�C��)
			break;                                           	//then �����{��
		system ("cls");                                         //�M��home_page�e��
		block.create_start ();                                  //�Ыز��ʤ�� & �U�Ӥ��
		block.get_time ();                                      //���o�C���}�l�ɶ�
		block.print ();                                         //��X�C���e��
		while (!game_over)                                      //�D�j��
		{
			if (!block.control ())                              //�I�scontrol �H���ʤ�� if (����Q��m)
			{
				if (!block.create ())                           //then �I�screate �H��m�s���ʤ�� if (�L�k��m�s���)
					game_over = 1;                              //then ���}�D�j��
				continue;
			}
			if (!(T % (11 - 2 * block.get_level ())) && !block.move_down ())    //if (�I�sget_level �p�⦹���O�_�ӤU��            {&&�����涶��}
			{                                                                   //	  �p�G�ӤU���h�I�smove_down �H�U����� �p�G����Q��m)
				if (!block.create ())                           //then �I�screate �H��m�s���ʤ�� if (�L�k��m�s���)
					game_over = 1;                              //then ���}�D�j��
				continue;
			}                                                   //���if����X�� �ݤ��}����
			block.print ();                                     //��X�C���e��
			Sleep (50);                                         //�Ȱ� �s�y�⦸���ʤ�������Ż�
			T ++;
			if (T > 100000)                                     //�קKoverflow
				T = 0;
		}
		if (!block.home_page (3))                               //if (������}�C��)
			break;                                           	//then �����{��
		record = block.get_highest_score ();                    //�x�s�̰���
	}
}

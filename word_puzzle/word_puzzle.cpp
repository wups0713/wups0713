#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>

#define ACROSS 0
#define DOWN 1

using namespace std;

enum Mode {by_order, random, by_priority};

/*parameters*/
Mode choose_variable_mode = by_priority;		//(by_order / random / by_priority)
Mode guess_word_mode = by_priority;				//(by_order / random / by_priority)
bool shuffle_readin_data = true;				//(true / false)
bool print_answer = true;						//(true / false)
int answers = 1;								//set -1 to do complete search
/*----------*/

/*global datas*/
string word_list[3000];
long long int visited_node = 0;
int case_num = 1;
int word_count = 0;
int variable_count = 0;
int ans_count = 0;
int max_x = 0;
int max_y = 0;
int max_len = 0;

int **times [26] = {0};							//the times of the character appear in words
												//ex:times [0][6][1] = 10
												//	 -->a([0]) appaer 10 times at second character([1])
												//		when the length of word is 6([6])
/*------------*/

/*structers*/															
struct Variable;
struct Linked_List;
struct Variable_List;
struct Cross_List;
/*---------*/

/*functions*/
void print_ans (Variable_List *variable_list);
void build_domain (Variable *variable);
void init_variable (Variable *variable, int start_x, int start_y, int length, char direction);
int count_value (Variable *variable, Linked_List *word);
void check_cross (Variable_List *variable_list);
Variable *choose_variable (Variable_List *variable_list);
bool guess (Variable *variable);
void domain_update (Variable_List *variable_list, Variable *variable);
void variable_reset (Variable *variable);
bool search (Variable_List *variable_list);
void free_struct (Variable_List *variable_list);
/*---------*/

struct Linked_List
{
    string *word;
    bool guessed;
    Linked_List *next;
};

struct Variable
{
	int start_x;
	int start_y;
    int len;
    bool direction;
    int total;
    bool guessed;
    string *word;
	Linked_List *domain;
	Cross_List *cross;
	int cross_count;
};

struct Cross_List
{
	Variable *variable;
	int pos1;
	int pos2;
	int len;
	Cross_List *next;
};

struct Variable_List
{
	Variable *variable;
	Variable_List *next;
};

int main (void)
{
	srand (time (NULL));
    ifstream words, puzzles;

	/*open file*/
    words.open ("English words 3000.txt");
    puzzles.open ("puzzle.txt");
    
    if (!words || !puzzles)
    {
        cout << "Fail to open file!";
        return 0;
    }
	/*---------*/
    
	/*read word list file*/
    while (!words.eof ())
    {
        words >> word_list[word_count];
        max_len = ((word_list[word_count].length() > max_len)?(word_list[word_count].length()):(max_len));
        word_count++;
    }

    words.close ();
	/*-------------------*/

	/*read puzzle file*/
    string line;
    Variable_List *variable_list = 0;
    while (getline (puzzles, line))
    {
        int start_x, start_y, length;
        char direction;
		stringstream ss;
        Variable_List* tail = 0;
		
        /*set variable*/
        ss << line;
        while (ss >> start_x >> start_y >> length >> direction)
        {
        	/*init variable data*/
            Variable_List *tmp = (Variable_List *)malloc (sizeof (Variable_List));
            tmp->variable = (Variable *)malloc (sizeof (Variable));
            init_variable (tmp->variable, start_x, start_y, length, direction);
            /*------------------*/
            
            /*shuffle variable order (on / off by shuffle_readin_data)*/
            if (shuffle_readin_data && variable_count)
            {
            	int random = 0;
				random = rand() % variable_count;
	            if (random)
	            {
	            	Variable_List *pre = variable_list;
	            	for (random--; random > 0; random--)
	            		pre = pre->next;
	            	tmp->next = pre->next;
	            	pre->next = tmp;
		    	}
		    	else
		    	{
					tmp->next = variable_list;
	            	variable_list = tmp;
				}
			}
			else
	    		if (tail)
	    		{
					tmp->next = tail->next;
		            tail->next = tmp;
		            tail = tmp;
				}
				else
				{
					tmp->next = variable_list;
					variable_list = tmp;
					tail = tmp;
				}
            
            variable_count++;
            /*------------------------------------------------------*/
        }
        check_cross (variable_list);
        /*------------*/
        
        /*start searching*/
		if (search (variable_list))
			print_ans (variable_list);
        else
        	if (!ans_count)
        		cout << "Fail to find an answer!\n";
        	else
        		cout << "Fail to find more answers!\n";
        /*---------------*/
		
		/*reset data*/
        free_struct (variable_list);
        variable_list = 0;
        
        visited_node = 0;
        variable_count = 0;
        ans_count = 0;
        max_x = 0;
        max_y = 0;
        case_num++;
		/*----------*/
    }

    puzzles.close ();
	/*----------------*/

	return 0;
}

bool search (Variable_List *variable_list)
{
	/*choose a variable*/
	Variable *choosen = choose_variable (variable_list);
	/*-----------------*/

	/*update domain*/
	domain_update (variable_list, choosen);		//decrease domain size by removing unsuitable words from domain
	/*-------------*/

	/*test the words in domain*/
	while (guess(choosen))
	{
		visited_node++;

		/*if find the answer then stop searching*/
		if (!variable_count)
		{
			if (answers == -1 || ans_count < answers - 1)
			{
				print_ans (variable_list);
				continue;
			}
			return 1;
		}
		/*--------------------------------------*/

		if (search (variable_list))
			return 1;
	}
	/*------------------------*/

	/*do trackback, reset variable's data*/
	variable_reset (choosen);
	/*-------------------*/
	
	return 0;
}

void print_ans (Variable_List *variable_list)
{
	/*print the answers*/
	if (!ans_count)
	{
    	cout << "#" << case_num << "\t(" << max_x << " x " << max_y << ")\n"
			 << "Answer(s):\n";
	}
	cout << "(" << ++ans_count << ")\n";
	
	char map [max_x + 1][max_y + 1];
	
	for(int i = 0; i < max_x + 1; i++)
		for (int j = 0; j < max_y + 1; j++)
			map [i][j] = ' ';
			
	Variable_List *tmp = variable_list;
	for (; tmp; tmp = tmp->next)
	{
		int length = tmp->variable->len, x = tmp->variable->start_x, y = tmp->variable->start_y;
		string *word = tmp->variable->word;
	
		if (tmp->variable->direction == ACROSS)
			for (int i = 0; i < length; i++)
				map [x + i][y] = ((word)?(*word)[i]:((map [x + i][y] > '*')?(map [x + i][y]):('*')));
		else
			for (int i = 0; i < length; i++)
				map [x][y + i] = ((word)?(*word)[i]:((map [x][y + i] > '*')?(map [x][y + i]):('*')));
	}
	
	if (print_answer)
		for(int i = 0; i < max_y + 1; i++)
		{
			for (int j = 0; j < max_x + 1; j++)
				cout << map [j][i];
			cout << endl;
		}
	cout << ((print_answer)?("\t"):("")) << "visited nodes:" << visited_node << endl << endl;
	/*-----------------*/
}

void build_domain (Variable *variable)
{
	/*put the words into domain if the length of word == the length of variable*/
	int count = 0;
	Linked_List *tail = 0;
    for (int i = 0; i < word_count; i++)
        if (word_list[i].length() == variable->len)
        {
            Linked_List *tmp = (Linked_List *)malloc (sizeof (Linked_List));
            tmp->word = &word_list[i];
            tmp->guessed = 0;
            
            /*shuffle word order (on / off by shuffle_readin_data)*/
            if (shuffle_readin_data && count)
            {
            	int random = 0;
            	random = rand() % count;
	            if (random)
	            {
	            	Linked_List *pre = variable->domain;
	            	for (random--; random > 0; random--)
	            		pre = pre->next;
	            	tmp->next = pre->next;
	            	pre->next = tmp;
		    	}
		    	else
		    	{
		    		tmp->next = variable->domain;
		    		variable->domain = tmp;
				}
			}
	    	else
	    		if (tail)
	    		{
					tmp->next = tail->next;
		            tail->next = tmp;
		            tail = tmp;
				}
				else
				{
					tmp->next = variable->domain;
					variable->domain = tmp;
					tail = tmp;
				}
				
			count ++;
			/*--------------------------------------------------*/
			
			/*calculate the times of cross*/
			string *word = tmp->word;
			int len = variable->len;
			for (int j = 0; j < len; j++)
			{
				int ch = (*word)[j] - 'a';

				if (!times [ch])
				{
					times [ch] = (int **)malloc (sizeof (int *) * max_len);
					for (int k = 0; k < max_len; k++)
						times [ch][k] = 0;
				}
				if (!times [ch][len - 1])
				{
					times [ch][len - 1] = (int*)malloc (sizeof (int) * len);
					for (int k = 0; k < len; k++)
						times [ch][len - 1][k] = 0;
				}
				times [ch][len - 1][j]++;
			}
			/*----------------------------*/
		}
	variable->total = count;
	/*-------------------------------------------------------------------------*/
}

void init_variable (Variable *variable, int start_x, int start_y, int length, char direction)
{
    /*calculate the size of puzzle*/
	int x = (direction == 'A')?(start_x + length - 1):(start_x);
	max_x = (max_x > x)?(max_x):(x);
	int y = (direction == 'D')?(start_y + length - 1):(start_y);
	max_y = (max_y > y)?(max_y):(y);
	/*----------------------------*/
	
	/*init data*/
	variable->start_x = start_x;
	variable->start_y = start_y;
    variable->len = length;
    variable->direction = (direction == 'A')?ACROSS:DOWN;
    variable->total = 0;
    variable->guessed = 0;
    variable->word = 0;
    variable->domain = 0;
    variable->cross = 0;
    variable->cross_count = 0;
	/*---------*/
    
    /*build the domain of the variable*/
    build_domain (variable);
    /*--------------------------------*/
}

int count_value (Variable *variable, Linked_List *word)
{
	/*calculate the value of words*/
	Cross_List *tmp = variable->cross;
	int value = 0;
	
	for (; tmp; tmp = tmp->next)
		for (int i = tmp->pos2; i < tmp->len; i++)
			value += times [(*word->word)[i] - 'a'][variable->len - 1][i];
	
	return value;
	/*----------------------------*/
}

void check_cross (Variable_List *variable_list)
{
	/*check if variables have cross each others and build Cross_List*/
	for (Variable_List *tmp1 = variable_list; tmp1; tmp1 = tmp1->next)
		for (Variable_List *tmp2 = tmp1->next; tmp2;tmp2 = tmp2->next)
		{
			bool direction1 = tmp1->variable->direction;
			bool direction2 = tmp2->variable->direction;
			int x1_head = tmp1->variable->start_x, x1_tail = x1_head + ((!direction1)?(tmp1->variable->len - 1):0);
			int x2_head = tmp2->variable->start_x, x2_tail = x2_head + ((!direction2)?(tmp2->variable->len - 1):0);
			int y1_head = tmp1->variable->start_y, y1_tail = y1_head + ((direction1)?(tmp1->variable->len - 1):0);
			int y2_head = tmp2->variable->start_y, y2_tail = y2_head + ((direction2)?(tmp2->variable->len - 1):0);
			
			if (!((x1_head > x2_tail) || (x1_tail < x2_head)))
				if (!((y1_head > y2_tail) || (y1_tail < y2_head)))
				{
					int pos1, pos2, len;
					if (direction1 != direction2)
					{
						pos1 = ((direction1)?(y2_head - y1_head):(x2_head - x1_head));
						pos2 = ((direction2)?(y1_head - y2_head):(x1_head - x2_head));
						len = 1;
					}
					else
					{
						int head1 = (direction1)?(y1_head):(x1_head), head2 = (direction2)?(y2_head):(x2_head);
						int tail1 = (direction1)?(y1_tail):(x1_tail), tail2 = (direction2)?(y2_tail):(x2_tail);
						pos1 = ((head1 > head2)?0:(head2 - head1));
						pos2 = ((head2 > head1)?0:(head1 - head2));
						int max_head = (head1 > head2)?(head1):(head2), min_tail = (tail1 > tail2)?(tail2):(tail1);
						len = min_tail - max_head + 1;
					}
					
					Cross_List *tmp = (Cross_List *)malloc (sizeof (Cross_List));
					tmp->variable = tmp2->variable;
					tmp->pos1 = pos1;	tmp->pos2 = pos2; tmp->len = len;
					tmp->next = tmp1->variable->cross;
					tmp1->variable->cross = tmp;
					tmp1->variable->cross_count++;
					
					tmp = (Cross_List *)malloc (sizeof (Cross_List));
					tmp->variable = tmp1->variable;
					tmp->pos1 = pos2;	tmp->pos2 = pos1; tmp->len = len;
					tmp->next = tmp2->variable->cross;
					tmp2->variable->cross = tmp;
					tmp2->variable->cross_count++;
				}
		}
	/*--------------------------------------------------------------*/
}

Variable *choose_variable (Variable_List *variable_list)
{
	Variable_List *tmp = variable_list;
	
	/*choose modes (select by choose_variable_mode)*/
	switch (choose_variable_mode)
	{
		case by_order:
		{
			for (; tmp; tmp = tmp->next)
				if (!tmp->variable->guessed)
				{
					tmp->variable->guessed = 1;
					variable_count--;
					return tmp->variable;
				}
			
			break;
		}
		case random:
		{
			int num = rand () % variable_count;
			for (; tmp; tmp = tmp->next)
				if (!tmp->variable->guessed)
					if (!num)
					{
						tmp->variable->guessed = 1;
						variable_count--;
						return tmp->variable;
					}
					else
						num--;
						
			break;
		}
		case by_priority:
		{
			Variable *min = 0;
			for (; tmp; tmp = tmp->next)
				if (!tmp->variable->guessed)
					if (min)
						min = ((min->cross_count > tmp->variable->cross_count)?(tmp->variable):(min));
					else
						min = tmp->variable;
			
			min->guessed = 1;
			variable_count--;
			return min;
			
			break;
		}
		default:
			break;
	}
	/*---------------------------------------------*/
}

bool guess (Variable *variable)
{
	Linked_List *tmp = variable->domain;
	
	/*if no more words can guess then do backtracking*/
	if (!variable->total)
		return 0;
	/*-----------------------------------------------*/
	
	/*guess modes (select by guess_word_mode)*/
	switch (guess_word_mode)
	{
		case by_order:
		{
			for (; tmp; tmp = tmp->next)
				if (!tmp->guessed)
				{
					tmp->guessed = 1;
					variable->word = tmp->word;
					variable->total--;
					return 1;
				}
		}
		case random:
		{
			int num = rand () % variable->total;
			for (; tmp; tmp = tmp->next)
				if (!tmp->guessed)
					if (!num)
					{
						tmp->guessed = 1;
						variable->word = tmp->word;
						variable->total--;
						return 1;
					}
					else
						num--;
		}
		case by_priority:
		{
			Linked_List *max = 0;
			int max_value = 0;
			for (; tmp; tmp = tmp->next)
				if (!tmp->guessed)
					if (max)
					{
						int value = count_value (variable, tmp);
						max = (value > max_value)?(tmp):(max);
					}
					else
					{
						max = tmp;
						max_value = count_value (variable, tmp);
					}
						
			max->guessed = 1;
			variable->word = max->word;
			variable->total--;
			return 1;
		}
		default:
			break;
	}
	/*---------------------------------------*/
}

void domain_update (Variable_List *variable_list, Variable *variable)
{
	char map [variable->len] = {0};
	
	/*remove words from domain*/
	Cross_List *tmp = variable->cross;
	for (; tmp; tmp = tmp->next)
		if (tmp->variable->word)
		{
			int start1 = tmp->pos1, start2 = tmp->pos2;
			for (int i = 0; i < tmp->len; i++)
				map [start1 + i] = (*(tmp->variable->word))[start2 + i];
		}
		else
			tmp->variable->cross_count--;
	
	Linked_List *temp = variable->domain;
	for (; temp; temp = temp->next)
	{
		string *word = temp->word;
		
		for (int i = 0; i < variable->len; i++)
		{
			if (map[i] && (*word)[i] != map[i])
			{
				temp->guessed = 1;
				variable->total--;
				break;
			}
		}
	}
	/*------------------------*/
}

void variable_reset (Variable *variable)
{
	variable_count++;
	
	variable->guessed = 0;
	variable->word = 0;
	
	/*reset domain*/
	Linked_List *tmp = variable->domain;
	for (; tmp; tmp = tmp->next)
		if (tmp->guessed)
		{
			tmp->guessed = 0;
			variable->total++;
		}
	/*------------*/

	Cross_List *temp = variable->cross;
	for (; temp; temp = temp->next)
		if (!temp->variable->guessed)
			temp->variable->cross_count++;
}

void free_struct (Variable_List *variable_list)
{
	/*free momery*/
	Variable_List *tmp = variable_list;
	Variable_List *del_variable;
	while (tmp)
	{
		Linked_List *tmp_domain = tmp->variable->domain;
		Linked_List *del_domain;
		while (tmp_domain)
		{
			del_domain = tmp_domain;
			tmp_domain = tmp_domain->next;
			free (del_domain);
		}
		Cross_List *tmp_cross = tmp->variable->cross;
		Cross_List *del_cross;
		while (tmp_cross)
		{
			del_cross = tmp_cross;
			tmp_cross = tmp_cross->next;
			free (del_cross);
		}
		del_variable = tmp;
		tmp = tmp->next;
		free (del_variable);
	}
	
	for (int i = 0; i < 26; i++)
		if (times [i])
		{
			for (int j = 0; j < max_len; j++)
				if (times [i][j])
				{
					free (times [i][j]);
					times [i][j] = 0;
				}
			free (times [i]);
			times [i] = 0;
		}
	/*-----------*/
}

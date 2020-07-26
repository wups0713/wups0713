#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include "random_forest.h"

using namespace std;

Data **read_data (char *filename, int *num_of_attr, int *num_of_data, int *num_of_class)
{
    /*get the number of attribute*/
    ifstream in(filename, ios_base::in);
    string line;
    stringstream s;
    double temp;
    
    getline (in, line);
    s << line;
    while (s >> temp)
    	(*num_of_attr)++;
    (*num_of_attr)--;
    
    in.close ();
    /*---------------------------*/
    
    /*read data set*/
    in.open (filename, ios_base::in);

	Data *data = 0;
    Data *tmp = 0;
    double *attrs = 0;

    while (getline (in, line))
    {
        stringstream ss;
        ss << line;

        if (data)
        {
            tmp->next = (Data *)malloc (sizeof (Data));
            tmp = tmp->next;
        }
        else
        {
            data = (Data *)malloc (sizeof (Data));
            tmp = data;
        }
        
        attrs = (double *)malloc (sizeof (double) * *num_of_attr);
        for (int i = 0; i < *num_of_attr; i++)
            ss >> attrs[i];
        
        tmp->attr = attrs;
        ss >> tmp->label;
        tmp->next = 0;

        *num_of_class = (tmp->label > *num_of_class)?tmp->label:*num_of_class;
        (*num_of_data)++;
    }
    
    in.close ();
    /*-------------*/
    
    /*turn linked list into array, for convenience*/
    Data **data_list = new Data*[*num_of_data];
    tmp = data;
    for (int i = 0; i < *num_of_data; i++)
    {
        data_list[i] = tmp;
        tmp = tmp->next;
    }
    /*--------------------------------------------*/ 
    
    return data_list;
}

void cut_data (Data **data, Data ***train, Data ***test, int start, int num_of_train, int num_of_test)
{
	/*cut data set into testing data and training data*/
	int num_of_data = num_of_train + num_of_test;
	*train = new Data*[num_of_train];
	*test = new Data*[num_of_test];
	
	for (int i = 0; i < start; i++)
		(*train)[i] = data[i];
		
	for (int i = start; i < start + num_of_test; i++)
		(*test)[i - start] = data[i];
		
	for (int i = start + num_of_test; i < num_of_data; i++)
		(*train)[i - num_of_test] = data[i];
	/*------------------------------------------------*/
}

void shuffle (Data **data, int num_of_data)
{
	/*shuffle data set order*/	
	for (int i = 0; i < num_of_data; i++)
	{
		int exchange = rand () % num_of_data;
		Data *tmp = data[i];
		data[i] = data[exchange];
		data[exchange] = tmp;
	}
	/*----------------------*/
}


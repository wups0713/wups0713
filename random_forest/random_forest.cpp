#include <iostream>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include "random_forest.h"

using namespace std;

int choosed_attr = 0;

Decision_Tree::Decision_Tree ()
{
	flag_print = 0;
	num_of_node = 0;
    num_of_data = 0;
    num_of_attr = 0;
    num_of_class = 0;
    depth_of_tree = 0;
    attr_list = 0;
    root = 0;
    data_list = 0;
}

void delete_node (Node *node)
{
	Node *left = node->left;
	Node *right = node->right;
	
	node->left = NULL;
	node->right = NULL;
	
	delete (node);
	
	if (left && right)
	{
		delete_node (left);
		delete_node (right);
	}
}

Decision_Tree::~Decision_Tree ()
{
	delete []attr_list;
	delete []data_list;
	delete_node (root);
}

bool Decision_Tree::build_tree (Data **data, int num1, int num2, bool *list, int num3, int num4, int num5)
{
    num_of_data = num1;
    num_of_attr = num2;
    num_of_class = num3;
    depth_of_tree = num4;
    size_of_node = num5;
    attr_list = list;
	root = new Node;
	data_list = data;

	root->ID = ++num_of_node;
    root->start = 0;
    root->threshold = -1;
    root->num_of_data = num_of_data;
    root->attr = -1;
    root->ans = -1;
    root->left = NULL;
    root->right = NULL;
    
    return split (root, depth_of_tree);
}

int cmp (const void *a, const void *b)
{
    Data *c = *(Data **)a;
    Data *d = *(Data **)b;

    if (c->attr[choosed_attr] < d->attr[choosed_attr])
        return -1;
    if (c->attr[choosed_attr] == d->attr[choosed_attr])
        return 0;
    if (c->attr[choosed_attr] > d->attr[choosed_attr])
        return 1;
}

void Decision_Tree::sort (Node *node)
{
    if (node->num_of_data > 1)
        qsort (data_list + node->start, node->num_of_data, sizeof (Data *), cmp);
}

double Decision_Tree::gini (int start, int total)
{
    int label[num_of_class];
    for (int i = 0; i < num_of_class; i++)
        label[i] = 0;
    
    for (int i = start; i < (start + total); i++)
        label[(data_list[i]->label - 1)]++;

    double p = 0, pk = 0;
    for (int i = 0; i < num_of_class; i++)
    {
        pk = (double)(label[i]) / (double)(total);
        p += pk * pk;
    }
    
    return ((double)1 - p);
}

bool Decision_Tree::split (Node *node, int depth)
{
    double min_G = 2, G, GA, GB;

	/*if the node's gini == 0, stop splitting*/
    G = gini (node->start, node->num_of_data);
    if (G >= 0 && G <=0)
    {
        node->ans = data_list[node->start]->label;
        if (flag_print)
        	print (node);
        return 1;
    }
    /*---------------------------------------*/
    
    /*if reach to the limit of depth or num_of_data <= size_of_node, stop splitting*/
    if (depth != 0 && node->num_of_data > size_of_node)
    {
		/*find the best threshold to split the node*/
	    for (int i = 0; i < num_of_attr; i++)
	        if (attr_list[i])
	        {
	            choosed_attr = i;
	            sort (node);
	
	            for (int j = 1; j < node->num_of_data; j++)
	            {
	                GA = gini (node->start, j);
	                GB = gini (node->start + j, node->num_of_data - j);
	                G = (double)j / (double)node->num_of_data * GA + (double) (node->num_of_data - j)/ (double)node->num_of_data * GB;
					if (G < min_G)
	                {
	                    min_G = G;
	                    node->attr = i;
	                    node->threshold = j;
	                }
	            }
	        }
		/*-----------------------------------------*/
	
		/*split left and right*/
	    choosed_attr = node->attr;
	    sort (node);
	    
	    Node *left = 0;
	    left = new Node;
	    left->ID = ++num_of_node;
	    left->start = node->start;
	    left->threshold = -1;
	    left->num_of_data = node->threshold;
	    left->attr = -1;
	    left->ans = -1;
	    left->left = NULL;
	    left->right = NULL;
	    node->left = left;
		
		Node *right = 0;
	    right = new Node;
	    right->ID = ++num_of_node;
		right->start = node->start + node->threshold;
	    right->threshold = -1;
	    right->num_of_data = node->num_of_data - node->threshold;
	    right->attr = -1;
	    right->ans = -1;
	    right->left = NULL;
	    right->right = NULL;
	    node->right = right;
	    
	    split (left, depth - 1);
		split (right, depth - 1);
		/*--------------------*/
	}
	/*-----------------------------------------------------------------------------*/
	
	/*debug*/
    if (flag_print)
    	print (node);
    /*-----*/
    
    return 1;
}

void Decision_Tree::print (Node *node)
{
	/*print all the datas in the node*/
	cout << node->ID << ": " << node->num_of_data << endl;
	for (int i = 0; i < node->num_of_data; i++)
	{
		cout << "\t" << i << "\t";
		for (int j = 0; j < num_of_attr; j++)
			cout << fixed << setprecision(6) << data_list[node->start + i]->attr[j] << "\t";
		cout << data_list[node->start + i]->label << endl;
	}
	/*-------------------------------*/

	/*print the node data*/
	if (node->ans == -1)
	{
		if (node->left)
			cout << node->ID << ":" 
				 << "\n\tattr " << node->attr
				 << "\n\tthreshold " << data_list[node->start + node->threshold]->attr[node->attr]
			 	 << "\n\tleft " << node->left->ID
				 << "\n\tright " << node->right->ID << endl;
		else
			cout << node->ID << ":" 
				 << "\n\tattr NULL"
				 << "\n\tthreshold NULL"
			 	 << "\n\tleft NULL"
				 << "\n\tright NULL" << endl;
	}
	else
		cout << node->ID << ":"
			 << "\n\tans " << node->ans << endl;
	/*-------------------*/
}

int Decision_Tree::start (double *attr)
{
    Node *tmp = root;
    int level = 0;

	/*go through the decision tree to find the answer*/
    while (tmp->ans == -1)
    {
        /*if reach to the end of tree, guess*/
        if (tmp->left == 0 && tmp->right == 0)
        	return guess (tmp);
        /*----------------------------------*/
		
		/*debug*/
		if (flag_print)
    		cout << "level: " << level 
				 << "\t\tID: " << tmp->ID
				 << "\t\tattr:" << tmp->attr << " " << data_list[tmp->start + tmp->threshold]->attr[choosed_attr]
				 << "\t";
    		
        choosed_attr = tmp->attr;
        
        if (attr[choosed_attr] < data_list[tmp->start + tmp->threshold]->attr[choosed_attr])
        {
        	if (flag_print)
        		cout << "left" << endl;
            tmp = tmp->left;
		}
        else
        {
        	if (flag_print)
        		cout << "right" << endl;
            tmp = tmp->right;
		}
		/*-----*/
    }
    /*-----------------------------------------------*/

    return tmp->ans;
}

int Decision_Tree::guess (Node *node)
{
	return data_list[node->start + (rand () % node->num_of_data)]->label;
}

void Decision_Tree::set_print (bool flag)
{
	flag_print = flag;
}

Random_Forest::Random_Forest ()
{
	flag_print = 0;
    num_of_tree = 0;
    num_of_data = 0;
    num_of_attr = 0;
    num_of_class = 0;
    depth_of_tree = 0;
    tree = 0;
}

Random_Forest::~Random_Forest ()
{
	delete []tree;
}

void Random_Forest::build_forest (Data **data, int num1, int num2, int num3, int num4, int num5, int num6)
{
    num_of_tree = num1;
    num_of_data = num2;
    num_of_attr = num3;
    num_of_class = num4;
    depth_of_tree = num5;
    size_of_node = num6;
    tree = new Decision_Tree[num_of_tree];

	/*build decision trees in random forest*/
    for (int i = 0; i < num_of_tree; i++)
    {
        bool *attr_list = new bool[num_of_attr];
        for (int j = 0; j < num_of_attr; j++)
            attr_list[j] = 1;
            
        /*randomly pick n^(1/2) ~ n datas from training data*/
        int num = random_pick (data);
        Data **tmp = new Data*[num];
        for (int j = 0; j < num; j++)
        	tmp[j] = data[j];
        /*--------------------------------------------------*/

		/*build trees*/
		tree[i].set_print (flag_print);
		tree[i].build_tree (tmp, num, num_of_attr, attr_list, num_of_class, depth_of_tree, size_of_node);
        /*-----------*/
    }
    /*-------------------------------------*/
}

int Random_Forest::random_pick (Data **data)
{
	shuffle (data);

	int tmp = (int)sqrt (num_of_data);
	return tmp + rand () % (num_of_data - tmp + 1);		//randomly pick n^(1/2) ~ n datas
}

void Random_Forest::shuffle (Data **data)
{
	for (int i = 0; i < num_of_data; i++)
	{
		int exchange = rand () % num_of_data;
		Data *tmp = data[i];
		data[i] = data[exchange];
		data[exchange] = tmp;
	}
}

double Random_Forest::start (Data **testing, int num_of_test)
{
	int correct = 0, ans = 0;
	
	/*go through all the decision trees and vote for the answer*/
	for (int i = 0; i < num_of_test; i++)
	{
		/*vote*/
		ans = vote (testing[i]->attr);
		/*----*/

		/*debug*/
		if (flag_print)
		{
			cout << "test: " << i << "\t\t" << testing[i]->attr[0] << "\t" << testing[i]->attr[1] << endl
				 << "ans:\t " << testing[i]->label << endl
				 << "predict: " << ans << endl << endl;
		}
		/*-----*/
		
		if (testing[i]->label == ans)
			correct++;
	}
	/*---------------------------------------------------------*/

	return (double)correct / (double)num_of_test;
}

int Random_Forest::vote (double *attr)
{
    int label[num_of_class];
    for (int i = 0; i < num_of_class; i++)
        label[i] = 0;
    
    /*vote*/
    for (int i = 0; i < num_of_tree; i++)
        label[tree[i].start (attr) - 1]++;
	/*----*/

	/*choose the highest one*/
    int max = 0;
    for (int i = 0; i < num_of_class; i++)
        max = (label[max] > label[i])?(max):(i);
    /*----------------------*/
    
    return max + 1;
}

void Random_Forest::set_print (bool flag)
{
	flag_print = flag;
}

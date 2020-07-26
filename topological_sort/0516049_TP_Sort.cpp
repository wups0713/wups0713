#include <iostream>
#include <fstream>
#define black	1
#define white   0
using namespace std;

struct node;

struct connect
{
	node *node;
	connect *next;
};

struct node
{
	int node;
	int color;
	connect *suc;
};

void DFS_visit (connect *suc, node *node, ofstream &outfile)
{
	while (suc)             //run through all succesors
	{
		if (!suc->node->color)              //if succesor's color is white
			DFS_visit (suc->node->suc, suc->node, outfile);     //do its succesor's DFS
		suc = suc->next;        //go to next succesor
	}
	node->color = black;            //after doing all of its succesor,turn it to black
	outfile << node->node << " ";           //out put to ans
}

void DFS (node nodes [], int node_count, ofstream &outfile)
{
	node *now_node;
	for (int i = 0;i < node_count;i++)        //run through all nodes
		{
			now_node = &nodes [i];          //let now_node go through all nodes
			if (!now_node->color)           //if now_node's color is white
				DFS_visit (now_node->suc, now_node, outfile);       //do now_node's DFS
		}
}

int main (void)
{
	ifstream infile ("sample data.txt");
	ofstream outfile ("TP_Sort_0516049.txt");
	int node_count = 0;
	while (infile >> node_count)
	{
		node nodes [node_count];
		for (int i = 0;i < node_count;i++)    //set nodes
		{                                       //
			infile >> nodes [i].node;           //load in the node's number
			nodes [i].color = 0;                //reset the color
			nodes [i].suc = NULL;               //reset the linklist of succesor
		}                                       //
		int lines = 0;
		infile >> lines;
		for (int i = 0;i < lines;i++)                               //connect the nodes
		{                                                           //
			int a = 0, b = 0;                                       //
			char ch;                                                //in order to ignore the ">" character
			infile >> a >> ch >> b;                                 //load in the nodes
			int j = 0, k = 0;                                       //
			for (;j < node_count && nodes [j].node != a;j++);     //searching for node
			for (;k < node_count && nodes [k].node != b;k++);     //searching for node
			connect *new_node = new connect;                        //
			new_node->node = &nodes [k];                            //adding b to a's linklist of succesor
			new_node->next = nodes [j].suc;                         //
			nodes [j].suc = new_node;                               //
		}
		DFS (nodes, node_count, outfile);
		outfile << endl;
	}
}


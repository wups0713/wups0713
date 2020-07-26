#include <iostream>

using namespace std;

struct Data
{
    double* attr;
    int label;
    Data* next;
};

struct Node
{
	int ID;
    int start;
    int threshold;
    int num_of_data;
    int attr;
    int ans;
    Node *left;
    Node *right;
};

class Decision_Tree
{
    public:
        Decision_Tree ();
        ~Decision_Tree ();

        bool build_tree (Data **, int, int, bool *, int, int, int);
        void sort (Node *);
        double gini (int, int);
        bool split (Node *, int);
        void print (Node *);
        int start (double *attr);
        int guess (Node *);
        void set_print (bool);

    private:
    	bool flag_print;
        int num_of_node;
        int num_of_data;
        int num_of_attr;
        int num_of_class;
        int depth_of_tree;
        int size_of_node;
        bool *attr_list;
        Node *root;
        Data **data_list;
};

class Random_Forest
{
	public:
		Random_Forest ();
		~Random_Forest ();

		void build_forest (Data **, int, int, int, int, int, int);
		int random_pick (Data **);
		void shuffle (Data **);
		double start (Data **, int);
        int vote (double *);
        void set_print (bool);

	private:
		bool flag_print;
		int num_of_tree;
		int num_of_data;
		int num_of_attr;
		int num_of_class;
		int depth_of_tree;
		int size_of_node;
		Decision_Tree *tree;
};

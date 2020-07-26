#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#include "main.h"

using namespace std;

char filename[30] = "cross200.txt";

//limit the depth of decision tree, set -1 to disable
const int depth_of_tree = -1;
//limit the size of decision tree node, set -1 to disable
const int size_of_node = -1;
//number of decision tree in random forest
const int num_of_tree = 20;
//testing data / all data
const double size_of_testing_data = (double)1 / (double)2;
//for K-fold cross validation
const int K = 10;
//run whole process for how many times
const int times = 10;

int main (void)
{
	srand (time (NULL));
	
	double final_accuracy_test = 0;
	double final_accuracy_train = 0;
	
	for (int repeat = 0; repeat < times; repeat++)
	{
		int num_of_attr = 0;
		int num_of_data = 0;
		int num_of_class = 0;
		int num_of_train = 0, num_of_test = 0, start = 0;
		Data **data = 0;
		Data **training = 0, **testing = 0;
	
		/*read file*/
		data = read_data (filename, &num_of_attr, &num_of_data, &num_of_class);
		shuffle (data, num_of_data);
		/*---------*/
	
		/*count testing data size*/
		if (K == 1)
			num_of_test = (int)((double)num_of_data * size_of_testing_data);
		else
			num_of_test = (int)((double)num_of_data / K);
		/*-----------------------*/
		
		/*K fold validation*/
		double accuracy_test = 0;
		double accuracy_train = 0;
		
		for (int i = 0; i < K; i++)
		{
			/*if K == 1, don't do K fold*/
			if (i == K - 1 && K != 1)
				num_of_test = num_of_data - start;
			/*--------------------------*/
				
			num_of_train = num_of_data - num_of_test;

			/*seperate data to training data & testing data*/
			cut_data (data, &training, &testing, start, num_of_train, num_of_test);
			/*---------------------------------------------*/

			/*build random forest*/
			Random_Forest random_forest;
			//random_forest.set_print (1);				//debug
			random_forest.build_forest (training, num_of_tree, num_of_train, num_of_attr, num_of_class, depth_of_tree, size_of_node);
			/*-------------------*/

			/*test random forest*/
			accuracy_test += random_forest.start (testing, num_of_test);
			accuracy_train += random_forest.start (training, num_of_train);
			/*------------------*/
			
			/*get next testing data*/
			start += num_of_test;
			/*---------------------*/
		}
		
		final_accuracy_test += accuracy_test / (double)K;
		final_accuracy_train += accuracy_train / (double)K;
		
		cout << "#" << repeat << "\t(" << K << "-fold)\n"
			 << "\tAccuracy of test: " << accuracy_test / (double)K << endl
			 << "\tAccuracy of train: " << accuracy_train / (double)K << endl;
		/*-----------------*/
	}
	
	cout << "Averge of " << times << " times " << K << "-fold cross-validation for testing data: " << final_accuracy_test / (double)times << endl;
	cout << "Averge of " << times << " times " << K << "-fold cross-validation for training data: " << final_accuracy_train / (double)times << endl;
}

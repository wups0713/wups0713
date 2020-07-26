#include <iostream>
#include <cstdlib>
#include "HashTable.h"

using namespace std;

HashTable::HashTable ()
{
	for (int i = 0;i < 10000;i++)
		drawers [i] = NULL;
}

HashTable::~HashTable()
{
}

void HashTable::addItem (string key, string gender, int height, int weight)
{
	int position = atoi (key.c_str()) % 10000;
	struct data *newdata = new struct data;
	newdata->key = key;
	newdata->gender = gender;
	newdata->height = height;
	newdata->weight = weight;
	newdata->next = drawers [position];
	drawers [position] = newdata;
}

HashTable &HashTable::operator [] (string key)
{
	now_key = key;
	return *this;
}

string HashTable::getGender ()
{
	struct data *search = drawers [atoi (now_key.c_str()) % 10000];
	while (1)
	{
		if (search->key == now_key)
			return search->gender;
		if (search->next == NULL)
		{
			cout << "Cannot find the data.\n";
   			return 0;
		}
		search = search->next;
	}
}

int HashTable::getHeight ()
{
	struct data *search = drawers [atoi (now_key.c_str()) % 10000];
	while (1)
	{
		if (search->key == now_key)
			return search->height;
		if (search->next == NULL)
		{
			cout << "Cannot find the data.\n";
  			return 0;
		}
		search = search->next;
	}
}

int HashTable::getWeight ()
{
	struct data *search = drawers [atoi (now_key.c_str()) % 10000];
	while (1)
	{
		if (search->key == now_key)
			return search->weight;
		if (search->next == NULL)
		{
			cout << "Cannot find the data.\n";
   			return 0;
		}
		search = search->next;
	}
}

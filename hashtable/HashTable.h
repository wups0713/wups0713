#include <iostream>

using namespace std;

struct data
{
	string key, gender;
	int height, weight;
	data *next;
};

class HashTable
{
	public:
		HashTable ();
		~HashTable ();
		void addItem (string, string, int, int);
		HashTable &operator [] (string);
		string getGender ();
		int getHeight ();
		int getWeight ();
	private:
		struct data *drawers [10000];
		string now_key;
};

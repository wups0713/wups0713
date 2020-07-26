#include <iostream>
#include "Link_List.h"
using namespace std;

Link_List::Link_List (int num)
{
	head = new Int_Node;
	head->value = num;
	head->pre = NULL;
	head->next = NULL;
	tail = head;
}//create a space saving (int)num

Link_List::Link_List (const Link_List &Link_List1)
{
	Int_Node *num1,*num2;
	num2 = new Int_Node;
	num1 = Link_List1.head;
	head = num2;
 	num2->next = NULL;
	num2->value = num1->value;
	while (num1->next != NULL)
	{
		Int_Node *temp = num2;
		num2->next = new Int_Node;
		num2 = num2->next;
		num1 = num1->next;
		num2->pre = temp;
		num2->value = num1->value;
	}
	num2->value = num1->value;
	//because the last Link_List will lost (the last num1->next must be NULL)
	num2->next = NULL;
	tail = num2;
}

Link_List::~Link_List()
{
	Int_Node *temp1,*temp2;
	temp1 = head;
	temp2 = head->next;
	while (temp1->next != NULL)
	{
		delete temp1;
		temp1 = temp2;
		temp2 = temp2->next;
	}
	delete temp1;
}

int Link_List::getSize () const
{
	int size=0;
	Int_Node *num;
	num = head;
	while (num != NULL)
	{
		size++;
		num = num->next;
	}
	//because the last Link_List will lost (the last num1->next must be NULL)
	return size;
}

const Link_List& Link_List::operator = (const Link_List &Link_List1)
{
	Link_List temp (Link_List1);
	head = temp.head;
}

bool Link_List::operator == (const Link_List &Link_List1) const
{
	Int_Node *num1,*num2;
	num1 = Link_List1.head;
	num2 = head;
	while (num1->next != NULL)
	{
		if (!(num1->value == num2->value))
			return 0;
		num1 = num1->next;
		num2 = num2->next;
	}
		if (!(num1->value == num2->value))
			return 0;
	return 1;
}

int &Link_List::operator [] (int place)
{
	Int_Node *num1;
	num1 = head;
	for (int i = 1;i < place;i++)
	{
		num1 = num1->next;
	}
	return num1->value;
}

int Link_List::operator [] (int place) const
{
	Int_Node *num1;
	num1 = head;
	for (int i = 1;i < place;i++)
	{
		num1 = num1->next;
	}
	return num1->value;
}

ostream& operator << (ostream& os,const Link_List &Link_List1)
{
	Int_Node *num;
	num = Link_List1.head;
	while (num != NULL)
	{
		os << num->value << " ";
		num = num->next;
	}
	return os;
}

istream& operator >> (istream & is, Link_List &Link_List1)
{
	Link_List1.head = new Int_Node;
	Link_List1.head->next = NULL;
	is >> Link_List1.head->value;
	Link_List1.tail = Link_List1.head;
	return is;
}

bool Link_List::insert_node (int num)
{
	tail->next = new Int_Node;
	Int_Node *temp;
	temp = tail;
	tail = tail->next;
	tail->pre = temp;
	tail->value = num;
	tail->next = NULL;
	return 1;
}

bool Link_List::insert_node (int place,int num)
{
	Int_Node *num1,*num2;
	num1 = head;
	for (int i = 1;i < place - 1;i++)
	{
		num1 = num1->next;
		if (num1->next == NULL)
			return 0;
	}
	Int_Node *front,*back;
	front = num1->next;
	back = num1;
	num2 = new Int_Node;
	num1->next = num2;
	num2->pre = back;
	num2->value = num;
	num2->next = front;
	front->pre = num2;
	return 1;
}

bool Link_List::delete_node ()
{
	Int_Node *temp;
	temp = tail;
	tail = tail->pre;
	tail->next = NULL;
	delete temp;
	return 1;
}

bool Link_List::delete_node(int place)
{
	Int_Node *num1;
	num1 = head;
	if (place == 1)
	{
		head = num1->next;
		return 1;
	}
	for (int i = 1;i < place;i++)
	{
		if (num1->next == NULL)
			return 0;
		num1 = num1->next;
	}
	num1->pre->next = num1->next;
	num1->next->pre = num1->pre;
	return 1;
}

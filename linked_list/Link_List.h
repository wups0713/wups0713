#ifndef _LINK_LIST
#define _LINK_LIST

#include <iostream>
using namespace std;

template <typename T>
struct Int_Node
{
	T value;
	Int_Node *pre, *next;
};

template <typename T>
class Link_List
{
	friend ostream &operator<<(ostream &os, const Link_List<T> &Link_List1)
	{
		Int_Node<T> *num;
		num = Link_List1.head;
		while (num != NULL)
		{
			os << num->value << " ";
			num = num->next;
		}
		return os;
	};
	friend istream &operator>>(istream &is, Link_List<T> &Link_List1)
	{
		Link_List1.head = new Int_Node<T>;
		Link_List1.head->next = NULL;
		is >> Link_List1.head->value;
		Link_List1.tail = Link_List1.head;
		return is;
	};

public:
	Link_List(int = 10);			//default constructor
	Link_List(const Link_List<T> &);	// copy constructor
	~Link_List();
	int getSize() const;
	
	const Link_List &operator=(const Link_List<T> &);		// assignment operator
	bool operator==(const Link_List<T> &) const;			// equality operator
	bool operator!=(const Link_List<T> &right) const		// inequality operator
	{
		return !(*this == right);
	}

	T &operator[](int);								// subscript operator for non-const objects
	T operator[](int) const;							// subscript operator for const objects

	bool insert_node(T);								// insert an integer at the back of link list
	bool delete_node();									// delete the last node
	bool insert_node(int , T);						// insert an integer after the i_th position
	bool delete_node(int);								// delete the i_th node

private:
	int size;
	Int_Node<T> *head, *tail;								// pointer to the first and the last element of Link_List
};

template <typename T>
Link_List<T>::Link_List (int num)
{
	head = new Int_Node<T>;
	head->value = num;
	head->pre = NULL;
	head->next = NULL;
	tail = head;
}

template <typename T>
Link_List<T>::Link_List (const Link_List<T> &Link_List1)
{
	Int_Node<T> *num1,*num2;
	num2 = new Int_Node<T>;
	num1 = Link_List1.head;
	head = num2;
 	num2->next = NULL;
	num2->value = num1->value;
	while (num1->next != NULL)
	{
		Int_Node<T> *temp = num2;
		num2->next = new Int_Node<T>;
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

template <typename T>
Link_List<T>::~Link_List()
{
	Int_Node<T> *temp1,*temp2;
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

template <typename T>
int Link_List<T>::getSize () const
{
	int size=0;
	Int_Node<T> *num;
	num = head;
	while (num != NULL)
	{
		size++;
		num = num->next;
	}
	//because the last Link_List will lost (the last num1->next must be NULL)
	return size;
}

template <typename T>
const Link_List<T>& Link_List<T>::operator = (const Link_List<T> &Link_List1)
{
	Link_List<T> temp (Link_List1);
	head = temp.head;
}

template <typename T>
bool Link_List<T>::operator == (const Link_List<T> &Link_List1) const
{
	Int_Node<T> *num1,*num2;
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

template <typename T>
T &Link_List<T>::operator [] (int place)
{
	Int_Node<T> *num1;
	num1 = head;
	for (int i = 1;i < place;i++)
	{
		num1 = num1->next;
	}
	return num1->value;
}

template <typename T>
T Link_List<T>::operator [] (int place) const
{
	Int_Node<T> *num1;
	num1 = head;
	for (int i = 1;i < place;i++)
	{
		num1 = num1->next;
	}
	return num1->value;
}

template <typename T>
bool Link_List<T>::insert_node (T num)
{
	tail->next = new Int_Node<T>;
	Int_Node<T> *temp;
	temp = tail;
	tail = tail->next;
	tail->pre = temp;
	tail->value = num;
	tail->next = NULL;
	return 1;
}

template <typename T>
bool Link_List<T>::insert_node (int place,T num)
{
	Int_Node<T> *num1,*num2;
	num1 = head;
	for (int i = 1;i < place - 1;i++)
	{
		num1 = num1->next;
		if (num1->next == NULL)
			return 0;
	}
	Int_Node<T> *front,*back;
	front = num1->next;
	back = num1;
	num2 = new Int_Node<T>;
	num1->next = num2;
	num2->pre = back;
	num2->value = num;
	num2->next = front;
	front->pre = num2;
	return 1;
}

template <typename T>
bool Link_List<T>::delete_node ()
{
	Int_Node<T> *temp;
	temp = tail;
	tail = tail->pre;
	tail->next = NULL;
	delete temp;
	return 1;
}

template <typename T>
bool Link_List<T>::delete_node(int place)
{
	Int_Node<T> *num1;
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

#endif // LINK_LIST

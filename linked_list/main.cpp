#include <iostream>

#include "Link_List.h"

using namespace std;

int main()
{

    // test default constructor
    Link_List<int> linkList1;
    // test copy constructor
    Link_List<int> linkList2(linkList1);

    // test getSize()
    cout << "linkList2 Size: " << linkList2.getSize() << endl;

    // test insert_node(value), delete_node(), operator<<, operator>>

    Link_List<float> linkList3;
	cout << "Enter a float num: ";
    cin >> linkList3;
	cout << "linkList3: "<< linkList3 << endl;

    linkList3.insert_node(11.0);
    linkList3.insert_node(12.5);
    linkList3.insert_node(13.0);
    linkList3.insert_node(14.5);
    linkList3.insert_node(15.0);
    cout << "Insert Boolean: " << linkList3.insert_node(16.5) << endl;
    cout << "linkList3: " << linkList3 << endl;

    cout << "Delete Boolean: " << linkList3.delete_node() << endl;
    cout << "linkList3: " << linkList3 << endl;


    // test assignment operator, equality operator, insert_node(index, value), delete_node(index)
    Link_List<float> linkList4 = linkList3;
    cout << "linkList4: " << linkList4 << endl;

    cout << "Insert Boolean: " << linkList4.insert_node(3, 17.0) << endl;
    cout << "linkList4: " << linkList4 << endl;

    cout << "Delete Boolean: " << linkList4.delete_node(4) << endl;
    cout << "Equality Boolean: " << (linkList4==linkList3) << endl;
    cout << "linkList4: " << linkList4 << endl;

    // test subscript operator []
    const Link_List<float> linkList5 = linkList4;
    cout << "linkList4[1]: " << linkList4[1] << endl;
    cout << "linkList5[1]: " << linkList5[1] << endl;

    Link_List<char> linkList6;
    linkList6.insert_node(65);
    linkList6.insert_node('P');
    linkList6.insert_node('P');
    linkList6.insert_node('L');
    linkList6.insert_node('E');
    cout << "linkList6" << linkList6 << endl;

	return 0;
}

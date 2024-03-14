// Project UID c1f28c309e55405daf00c565d57ff9ad

#include "List.h" // check for proper include guards

#include <iostream>

using std::cout;
using std::endl;

// THIS TEST CASE WILL ONLY TELL YOU IF YOUR CODE COMPILES.
// YOU NEED TO WRITE YOUR OWN COMPREHENSIVE TEST CASES IN List_tests.cpp

int main()
{
    List<int> my_list;

    int size = 0;
    my_list.empty();
    my_list.size();
    cout << (my_list.size() == size) << endl;

    my_list.push_front(42);
    my_list.push_back(42);

    my_list.front() = 73;
    my_list.back() = 73;

    my_list.pop_front();
    my_list.pop_back();

    List<int> list2(my_list);
    list2 = my_list;

    List<int>::Iterator iter = my_list.begin();
    my_list.insert(iter, 42);
    iter = my_list.begin();
    my_list.erase(iter);
    iter = my_list.begin();

    List<int>::Iterator iter2 = my_list.end();
    const List<int>::Iterator iter3 = iter2;

    bool b = iter == iter2;
    b = iter3 == my_list.end();
    b = my_list.end() == iter3;
    my_list.push_front(42);
    my_list.push_back(42);
    iter = my_list.begin();
    b = iter != iter2;
    b = iter3 != my_list.end();
    b = my_list.end() != iter3;
    cout << b << endl;
    ++iter;

    cout << *iter << endl;

    List<int>::Iterator iter_cpy(iter);
    iter_cpy = iter;

    cout << *++--iter << endl;
    cout << *--++my_list.begin() << endl;

    my_list.insert(my_list.begin(), 3);
    my_list.erase(my_list.begin());

    List<int> list3;
    list3.push_back(3);
    cout << *list3.begin() << endl;

    List<int> list4;
    list4.push_back(4);
    cout << list4.size() << endl;

    my_list.clear();

    return 0;
}

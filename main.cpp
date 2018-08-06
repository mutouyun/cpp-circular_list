#include <iostream>
#include "circular_list.h"

using namespace std;

int main(void)
{
    unsigned char buff[sizeof(int) * 10];
    circular_list<int> list(buff, sizeof(buff));
    list.push_back(123);
    list.pop_front();

    for (size_t i = 0; i < list.max_count(); ++i)
    {
        list.push_back(i);
    }

    for (int v : list) std::cout << v << " ";
    std::cout << std::endl;

    list.pop_front();
    for (int v : list) std::cout << v << " ";
    std::cout << std::endl;

    list.pop_back();
    for (int v : list) std::cout << v << " ";
    std::cout << std::endl;

    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    list.push_front(4);
    for (int v : list) std::cout << v << " ";
    std::cout << std::endl;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);
    for (int v : list) std::cout << v << " ";
    std::cout << std::endl;

    unsigned char buff2[sizeof(int) * 100];
    circular_list<int> list2(buff2, sizeof(buff2));

    list2 << list;
    for (int v : list2) std::cout << v << " ";
    std::cout << std::endl;

    return 0;
}

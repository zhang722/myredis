#include <cassert>
#include <iostream>

#include "Skiplist.h"

void skiplistTest() {
    myredis::Skiplist<int, int> list(32); 

    list.insert(1, 10);
    list.print();
    list.insert(2, 20);
    list.insert(4, 40);
    list.insert(8, 80);
    list.insert(16, 160);
    assert(list.size() == 5);    

    list.remove(4, 40);
    assert(list.size() == 4);    

    list.print();
    assert(list.isInRange(10, 160));
    list.remove(8, 80);
    assert(!list.isInRange(70, 90));

    assert(list.firstInRange(10, 16)->val_ == 10);
    assert(list.lastInRange(10, 160)->val_ == 160);

    std::cout << "Skiplist test passed." << std::endl;    
}
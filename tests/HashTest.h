#include <cassert>
#include <iostream>

#include "Hash.h"

void hashTest() {
    myredis::Hash<int, int> h;
    for (int i = 0; i < 1000; i++) {
        h.insert(i, i);
    }
    assert(h.size() == 1000);    
    assert(h[20] == 20);
    std::cout << "Hash test passed." << std::endl;    
}
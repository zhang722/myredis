#pragma once

#include <iostream>

#include "Intset.h"

void intsetTest() {
    std::cout << "Intset test" << std::endl;
    myredis::Intset set;
    set.insert(1);

    set.print();
    set.insert(65536);
    set.print();
}
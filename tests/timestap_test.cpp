// File:    timestap_test.cpp
// Author:  definezxh@163.com
// Date:    2019/07/01 16:50:37
// Desc:
//   Timestap test.

#include <Timestap.h>
#include <iostream>

using namespace hquin;

int main() {
    Timestap stap = Timestap::now();
    std::cout << stap.stringifyTimestap() << std::endl;
    std::cout << stap.formatTimestap() << std::endl;
}
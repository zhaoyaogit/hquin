// File:    timestap_test.cpp
// Author:  definezxh@163.com
// Date:    2019/07/01 16:50:37
// Desc:
//   Timestamp test.

#include <Timestamp.h>
#include <iostream>

using namespace hquin;

int main() {
    Timestamp stap = Timestamp::now();
    std::cout << stap.formatTimestamp() << std::endl;
    std::cout << stap.formatTimestamp() << std::endl;
}
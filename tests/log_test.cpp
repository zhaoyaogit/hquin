// File:    log_test.cpp
// Author:  definezxh@163.com
// Date:    2019/07/04 15:00:03
// Desc:
//   logger test.

#include <Log.h>
#include <Timestap.h>

#include <iostream>

using namespace hquin;

int foo(int a) { return a < 1 ? 1 : 0; }

int main() {

    const uint32_t kSecondConv = 1000;

    char ch = 'a';
    int32_t i32 = 1;
    uint32_t u32 = 10;
    int64_t i64 = 100;
    uint64_t u64 = 1000;
    double df = 3.1314;
    const char *cstr = "hello @c string";
    std::string str = "hello std::string";

    uint64_t longstart = Timestap::now().timestap();
    for (uint32_t i = 0; i < 100000; ++i)
        LOG_INFO << ch << ' ' << i32 << ' ' << u32 << ' ' << i64 << ' ' << u64
                 << ' ' << df << cstr << str << ' ' << i;
    uint64_t longend = Timestap::now().timestap();

    std::cout << "total time: " << longend - longstart
              << ", avg time: " << (longend - longstart) * kSecondConv / 100000
              << '\n';

    uint64_t shortstart = Timestap::now().timestap();
    for (uint32_t i = 0; i < 100000; ++i)
        LOG_INFO << ch;
    uint64_t shortend = Timestap::now().timestap();

    std::cout << "total time: " << shortend - shortstart << "us , avg time: "
              << (shortend - shortstart) * kSecondConv / 100000 << '\n';
}
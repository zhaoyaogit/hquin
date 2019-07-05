// File:    log_test.cpp
// Author:  definezxh@163.com
// Date:    2019/07/04 15:00:03
// Desc:
//   logger test.

#include <Log.h>

using namespace hquin;

int foo(int a) { return a < 1 ? 1 : 0; }

int main() {
    char ch = 'a';
    int32_t i32 = 1;
    uint32_t u32 = 10;
    int64_t i64 = 100;
    uint64_t u64 = 1000;
    double df = 3.1314;
    const char *cstr = "hello @c string";
    std::string str = "hello std::string";
    for (int i = 0; i < 10; i++)
        str += "hello std::string hello std::string hello std::string hello "
               "std::stringhello std::string";

    for (uint32_t i = 0; i < 1000; ++i)
        LOG_INFO << ch << ' ' << i32 << ' ' << u32 << ' ' << i64 << ' ' << u64
                 << ' ' << df << cstr << str << ' ' << i;
}
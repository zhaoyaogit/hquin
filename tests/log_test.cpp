// File:    log_test.cpp
// Author:  definezxh@163.com
// Date:    2019/07/04 15:00:03
// Desc:
//   logger test.

#include <Log.h>

using namespace hquin;

int main() {
    LOG_INFO << "LOG_INFO";
    LOG_WARN << "LOG_WARN";
    LOG_ERROR << "LOG_ERROR";
}
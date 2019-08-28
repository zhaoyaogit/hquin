// File:    Util.h
// Author:  definezxh@163.com
// Date:    2019/08/28 09:22:05
// Desc:
//   Concentrate some basic function.

#include <Util.h>

__thread pid_t tid = 0;

namespace hquin {

pid_t gettid() {
    if (tid == 0)
        tid = syscall(__NR_gettid);
    return tid;
}

} // namespace hquin

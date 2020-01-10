// File:    Util.h
// Author:  definezxh@163.com
// Date:    2019/08/28 09:22:05
// Desc:
//   Concentrate some basic function.

#pragma once

#include <sys/syscall.h>
#include <unistd.h>

namespace hquin {

pid_t gettid();

} // namespace hquin

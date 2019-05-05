// File:    Log.h
// Author:  definezxh@163.com
// Date:    2019/05/05 17:06:57
// Desc:
//   Simple Log printer.

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

namespace hquin {

static void doLog(int errnoflag, int error, const char *fmt, va_list ap) {
    char buf[128];
    vsnprintf(buf, 127, fmt, ap);
    size_t len = strlen(buf);
    if (errnoflag)
        snprintf(buf + len, 127 - len, ": %s", strerror(error));
    strcat(buf, "\n");
    fflush(stdout); // in case stdout and stderr are the same.
    fputs(buf, stderr);
    fflush(NULL); // flushes all stdio output streams.
}

void logMsg(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    doLog(0, 0, fmt, ap);
    va_end(ap);
}

void logExit(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    doLog(1, errno, fmt, ap);
    va_end(ap);
    exit(1);
}

} // namespace hquin

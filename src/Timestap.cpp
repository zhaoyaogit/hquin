// File:    Timestap.cpp
// Author:  definezxh@163.com
// Date:    2019/07/01 17:15:16
// Desc:
//   Wrapper of timestap.

#include <Timestap.h>

#include <sys/time.h>

#include <stdio.h>

namespace hquin {

Timestap Timestap::now() {
    // use gettimeofday(2) is 15% faster then std::chrono
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t timestap = tv.tv_sec * 1000000 + tv.tv_usec;

    return Timestap(timestap);
}

std::string Timestap::formatTimestap() const {
    time_t second = timestap_ / 1000000 + /* CST */ 8 * 3600;
    auto time = gmtime(&second);
    char buffer[32], format[40];
    strftime(buffer, 32, "%Y/%m/%d %T.", time);
    snprintf(format, sizeof(format), "%s%06lu", buffer, timestap_ % 1000000);
    return format;
}

std::string Timestap::stringifyTimestap() const {
    return std::to_string(timestap_);
}

} // namespace hquin
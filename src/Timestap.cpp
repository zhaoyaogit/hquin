// File:    Timestap.cpp
// Author:  definezxh@163.com
// Date:    2019/07/01 17:15:16
// Desc:
//   Wrapper of timestap.

#include <Timestap.h>

#include <time.h>
#include <stdio.h>

namespace hquin {

Timestap Timestap::now() {
    uint64_t timestap =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    return Timestap(timestap);
}

std::string Timestap::formatTimestap() const {
    std::time_t time_t = timestap_ / 1000000;
    auto gmtime = std::gmtime(&time_t);
    char buffer[32], format[40];
    strftime(buffer, 32, "%Y/%m/%d %T.", gmtime);
    snprintf(format, sizeof(format), "%s%06lu", buffer, timestap_ % 1000000);
    return format;
}

std::string Timestap::stringifyTimestap() const {
    return std::to_string(timestap_);
}

} // namespace hquin

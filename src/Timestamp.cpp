// File:    Timestamp.cpp
// Author:  definezxh@163.com
// Date:    2019/07/01 17:15:16
// Desc:
//   Wrapper of timestamp.

#include <Timestamp.h>

#include <sys/time.h>

#include <stdio.h>

__thread time_t second;
__thread char formatTime[18]; // 20190816 15:32:25

namespace hquin {

const uint32_t Timestamp::kMicrosecondOneSecond = 1000000;

Timestamp Timestamp::now() {
    // use gettimeofday(2) is 15% faster then std::chrono
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t timestamp = tv.tv_sec * kMicrosecondOneSecond + tv.tv_usec;

    return Timestamp(timestamp);
}

std::string Timestamp::time() const {
    time_t nowsec = timestap_ / kMicrosecondOneSecond + /* CST */ 8 * 3600;
    if (second < nowsec) {
        second = nowsec;
        struct tm *time = gmtime(&second);
        strftime(formatTime, sizeof(formatTime), "%Y%m%d %T", time);
    }

    return formatTime;
}

std::string Timestamp::date() const { return std::string(time(), 0, 8); }

std::string Timestamp::formatTimestamp() const {
    char format[28];
    snprintf(format, sizeof(format), "%s.%06lu", time().c_str(),
             timestap_ % kMicrosecondOneSecond);
    return format;
}

} // namespace hquin
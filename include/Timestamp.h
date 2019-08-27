// File:    Timestamp.h
// Author:  definezxh@163.com
// Date:    20190701 15:59:58
// Desc:
//   Wrapper of timestamp.

#pragma once

#include <string>

namespace hquin {

class Timestamp {
  public:
    Timestamp() : timestap_(0) {}
    Timestamp(uint64_t timestamp) : timestap_(timestamp) {}

    // Timestamp of current time.
    static Timestamp now();

    // 20180805
    std::string date() const;

    // 20180805 14:45:20
    std::string time() const;

    // format: 20190701 16:28:30.070981
    std::string formatTimestamp() const;

    uint64_t timestamp() const { return timestap_; }

    static const uint32_t kMicrosecondOneSecond;

  private:
    uint64_t timestap_;
};

} // namespace hquin

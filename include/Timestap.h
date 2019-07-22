// File:    Timestap.h
// Author:  definezxh@163.com
// Date:    2019/07/01 15:59:58
// Desc:
//   Wrapper of timestap.

#pragma once

#include <string>

namespace hquin {

class Timestap {
  public:
    Timestap() : timestap_(0) {}
    Timestap(uint64_t timestap) : timestap_(timestap) {}

    // Timestap of current time.
    static Timestap now();

    // format: 2019/07/01 16:28:30.070981
    std::string formatTimestap() const;

    std::string stringifyTimestap() const;

    uint64_t timestap() const { return timestap_; }

  private:
    uint64_t timestap_;
};

} // namespace hquin

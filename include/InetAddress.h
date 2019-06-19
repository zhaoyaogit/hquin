// File:    InetAddress.h
// Author:  definezxh@163.com
// Date:    2019/06/17 19:09:15
// Desc:
//   IPv4 internet socket address structure.

#pragma once

#include <netinet/in.h>

#include <string>

namespace hquin {

class InetAddress {
  public:
    // constructs a endpoint with given port number,
    // mostly used in TCPServer listening.
    explicit InetAddress(uint16_t port);

    // constructs a endpoint with given sockaddr_in structure,
    // used in new connnection.
    InetAddress(const struct sockaddr_in &addr);

    // copy assignment of socket address
    const struct sockaddr_in &getSockAddrInet() const { return addr_; }
    void setSockAddrInet(const struct sockaddr_in &addr) { addr_ = addr; }

    // convert network binay to string form (addr:port).
    std::string stringifyHost();

  private:
    struct sockaddr_in addr_;
};

} // namespace hquin

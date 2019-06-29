// File:    InetAddress.h
// Author:  definezxh@163.com
// Date:    2019/06/17 19:09:15
// Desc:
//   IPv4 internet socket address structure.
//   Concentrate operations related to some socket function of parameter is
//   'struct sockaddr_in'.

#pragma once

#include <netinet/in.h>

#include <string>

namespace hquin {

class InetAddress {
  public:
    // constructs a endpoint with give port number and address
    // mostly used in new connection.
    InetAddress(uint16_t port, std::string address);

    // constructs a endpoint with given sockaddr_in structure,
    // used in new connnection.
    InetAddress(const struct sockaddr_in &addr);

    // constructs a endpoint with given port number,
    // mostly used in TCPServer listening.
    explicit InetAddress(uint16_t port);

    // copy assignment of socket address
    const struct sockaddr_in &getSockAddrInet() const { return inetAddr_; }
    void setSockAddrInet(const struct sockaddr_in &addr) { inetAddr_ = addr; }

    // convert network binay to string form (addr:port).
    std::string stringifyHost() const;

    uint16_t getPort() const { return inetAddr_.sin_port; }

    // bind(2)
    void bindSockAddrInet(int sockfd);

    // accept(2)
    int acceptSockAddrInet(int sockfd);

  private:
    struct sockaddr_in inetAddr_;
};

} // namespace hquin

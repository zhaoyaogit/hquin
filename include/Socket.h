// File:    Socket.h
// Author:  definezxh@163.com
// Date:    2019/06/30 22:13:06
// Desc:
//   Wrapper of socket file descriptor.

#pragma once

namespace hquin {

class InetAddress;

// socket series function is same as socket(2).
class Socket {
  public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {}
    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;
    ~Socket();

    int fd() const { return sockfd_; }

    // it should be abort if bind error.
    void bindAddress(const InetAddress &localAddr);

    // it should be abort if listen error.
    void listen();

    // not const, peerAddr will be change in accept(2).
    int accept(InetAddress *peerAddr);

    // close one end of writing.
    void shutdownWrite();

  private:
    const int sockfd_; // shoulde not be change.
};

} // namespace hquin

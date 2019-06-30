// File:    Socket.cpp
// Author:  definezxh@163.com
// Date:    2019/06/30 22:22:25
// Desc:
//   Wrapper of socket file descriptor.

#include <Socket.h>
#include <InetAddress.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace hquin {

Socket::~Socket() { ::close(sockfd_); }

void Socket::bindAddress(const InetAddress &localAddr) {
    ::bind(sockfd_, (struct sockaddr *)&localAddr, sizeof(localAddr));
}

void Socket::listen() { ::listen(sockfd_, SOMAXCONN); }

int Socket::accept(InetAddress *peerAddr) {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int connfd = ::accept(sockfd_, (struct sockaddr *)&addr, &len);
    if (connfd > 0) {
        peerAddr->setSockAddrInet(addr);
    }
    return connfd;
}

} // namespace hquin

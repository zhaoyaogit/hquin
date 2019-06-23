// File:    Acceptor.cpp
// Author:  definezxh@163.com
// Date:    2019/06/19 19:14:34
// Desc:
//   Acceptor corresponding to each connection.

#include <Acceptor.h>

#include <sys/socket.h>
#include <unistd.h>

namespace hquin {

// complete necessary operation of network io, include socket(), bind(),
// and set the callback fucntion when new connection arrived could exec.
Acceptor::Acceptor(EventLoop *eventloop, const InetAddress &addr)
    : eventloop_(eventloop),
      sockfd_(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)),
      acceptChannel_(eventloop_, sockfd_), servaddr_(addr), listenning_(false) {
    servaddr_.bindSockAddrInet(sockfd_);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

// call listen() is ready to read.
void Acceptor::listen() {
    listenning_ = true;
    acceptChannel_.enableReadable();
    ::listen(sockfd_, SOMAXCONN);
}

// callback function.
void Acceptor::handleRead() {
    InetAddress newConnAddr(servaddr_);
    int connfd = newConnAddr.acceptSockAddrInet(sockfd_);
    if (connfd >= 0) {
        newConnectionCallback_(connfd, newConnAddr);
    } else {
        ::close(connfd);
    }
}

} // namespace hquin

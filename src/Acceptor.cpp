// File:    Acceptor.cpp
// Author:  definezxh@163.com
// Date:    2019/06/19 19:14:34
// Desc:
//   Acceptor corresponding to each connection.

#include <Acceptor.h>
#include <InetAddress.h>

#include <sys/socket.h>
#include <unistd.h>

namespace hquin {

// Complete necessary operation of network io, include socket(), bind(),
// A pivotal reason to call these socket function here could see
// `Acceptor::listen()`.
Acceptor::Acceptor(EventLoop *eventloop, const InetAddress &listenAddr)
    : eventloop_(eventloop),
      acceptSocket_(
          ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0)),
      acceptChannel_(eventloop_, acceptSocket_.fd()), listenning_(false) {
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() { ::close(acceptSocket_.fd()); }

// Update channel to epoll events must after listen() what maintaining two
// queues. It is ease to cause the code confusion if call the listen() on the
// Acceptor Owner(TcpServer) but call enable*() here.
void Acceptor::listen() {
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReadable();
}

// Callback function.
void Acceptor::handleRead() {
    InetAddress peerAddr(0);
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0) {
        newConnectionCallback_(connfd, peerAddr);
    } else {
        ::close(connfd);
    }
}

} // namespace hquin

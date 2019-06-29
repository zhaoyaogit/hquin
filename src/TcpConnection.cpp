// File:    TcpConnection.cpp
// Author:  definezxh@163.com
// Date:    2019/06/29 13:51:43
// Desc:
//

#include <TcpConnection.h>
#include <EventLoop.h>
#include <Channel.h>

#include <unistd.h>

#include <stdio.h>
#include <string.h>

namespace hquin {

TcpConnection::TcpConnection(EventLoop *loop, std::string name, int sockfd,
                             const InetAddress &peerAddr)
    : eventloop_(loop), name_(name), state_(kConnecting),
      channel_(std::make_unique<Channel>(eventloop_, sockfd)),
      peerAddr_(peerAddr) {
    printf("TcpConnection ctor %s at %p, fd = %d\n", peerAddr_.stringifyHost().c_str(), this,
           sockfd);
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection() {
    printf("TcpConnection dctor %s at %p, fd = %d\n", name_.c_str(), this,
           channel_->fd());
}

void TcpConnection::connectEstablished() {
    setState(kConnected);
    channel_->enableReadable();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    setState(kDisconnected);
    channel_->disableAll();
    eventloop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead() {
    char buf[65536];
    size_t n = ::read(channel_->fd(), buf, sizeof(buf));
    if (n > 0) {
        messageCallback_(shared_from_this(), buf, n);
    } else if (n == 0) {
        handleClose();
    } else {
        handleError();
    }
}

void TcpConnection::handleWrite() {}

void TcpConnection::handleClose() {
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError() {
    printf("simpele handle: error: %s\n", strerror(errno));
    exit(-1);
}

} // namespace hquin

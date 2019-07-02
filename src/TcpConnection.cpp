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

using namespace std::placeholders;

namespace hquin {

TcpConnection::TcpConnection(EventLoop *loop, std::string name, int sockfd,
                             const InetAddress &peerAddr)
    : eventloop_(loop), name_(name), state_(kConnecting),
      channel_(std::make_unique<Channel>(eventloop_, sockfd)),
      peerAddr_(peerAddr), inputBuffer_() {
    printf("TcpConnection ctor %s at %p, fd = %d\n",
           peerAddr_.stringifyHost().c_str(), this, sockfd);
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, _1));
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

void TcpConnection::handleRead(Timestap receiveTime) {
    int saveErrno = 0;
    const int n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
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

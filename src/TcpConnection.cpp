// File:    TcpConnection.cpp
// Author:  definezxh@163.com
// Date:    2019/06/29 13:51:43
// Desc:
//

#include <TcpConnection.h>
#include <Log.h>
#include <Socket.h>
#include <EventLoop.h>
#include <Channel.h>

#include <unistd.h>

#include <string.h>

using namespace std::placeholders;

namespace hquin {

TcpConnection::TcpConnection(EventLoop *loop, std::string name, int sockfd,
                             const InetAddress &peerAddr)
    : eventloop_(loop), name_(name), state_(kConnecting),
      channel_(std::make_unique<Channel>(eventloop_, sockfd)),
      socket_(std::make_unique<Socket>(sockfd)), peerAddr_(peerAddr) {
    LOG_INFO << "TcpConnection ctor " << peerAddr_.stringifyHost()
             << ", fd = " << sockfd;
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}

TcpConnection::~TcpConnection() {
    LOG_INFO << "TcpConnection dtor " << name_ << ", fd = " << channel_->fd();
}

// send data to peer socket, save data to outbuffer first.
void TcpConnection::send(const std::string &message) {
    outputBuffer_.append(message);
    channel_->enableWriting();
}

void TcpConnection::connectEstablished() {
    setState(kConnected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

// half-close end of server writing.
void TcpConnection::shutdown() {
    if (state_ == kConnected) {
        setState(kDisconnecting);
        if (!channel_->isWriting()) {
            socket_->shutdownWrite();
        }
    }
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

// write data to peer socket fd from output buffer.
// reduce fd use.
void TcpConnection::handleWrite() {
    if (channel_->isWriting()) {
        ssize_t n = ::write(channel_->fd(), outputBuffer_.beginRead(),
                            outputBuffer_.readableBytes());
        if (n > 0) {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0) {
                shutdown();
            }
        } else {
            // LOG_ERROR << "TcpConnection::handleWrite";
        }
    }
}

void TcpConnection::handleClose() {
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError() { LOG_ERROR << strerror(errno); }

} // namespace hquin

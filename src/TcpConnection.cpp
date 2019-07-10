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
    LOG_WARN << "TcpConnection dtor " << name_ << ", fd = " << channel_->fd();
}

// send data to peer socket.
// try to send to socket fd, put the reamain data into output buffer, if
// write(2) not send completely.
void TcpConnection::send(const std::string &message) {
    // FIXME: multiple thread shoule do in loop.
    if (state_ == kConnected) {
        ssize_t nwrote =
            ::write(channel_->fd(), message.data(), message.size());
        if (nwrote >= 0) {
            if (static_cast<size_t>(nwrote) < message.size()) {
                LOG_INFO << "write data to fd is not complete.";
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_ERROR << "TcpConnection::sendInLoop";
            }
        }

        if (static_cast<size_t>(nwrote) < message.size()) {
            outputBuffer_.append(message.data() + nwrote);
            if (!channel_->isWriting()) {
                channel_->enableWriting();
            }
        }
    }
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
        eventloop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop() {
    if (!channel_->isWriting())
        socket_->shutdownWrite();
}

void TcpConnection::connectDestroyed() {
    setState(kDisconnected);
    channel_->disableAll();
    connectionCallback_(shared_from_this());
    eventloop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead(Timestap receiveTime) {
    int saveErrno = 0;
    const int n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose(); // unreadble.
    } else {
        handleError();
    }
}

// write data to peer socket fd from output buffer.
// reduce fd use.
void TcpConnection::handleWrite() {
    if (channel_->isWriting()) {
        LOG_INFO << "handleWrite";
        ssize_t n = ::write(channel_->fd(), outputBuffer_.beginRead(),
                            outputBuffer_.readableBytes());
        if (n > 0) {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if (state_ == kDisconnecting)
                    shutdown();
            } else {
                LOG_INFO << "I am going to write more data";
            }
        } else {
            LOG_ERROR << "TcpConnection::handleWrite";
        }
    }
}

void TcpConnection::handleClose() {
    assert(state_ == kConnected || state_ == kDisconnecting);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError() { LOG_ERROR << strerror(errno); }

} // namespace hquin

// File:    TcpConnection.h
// Author:  definezxh@163.com
// Date:    2019/06/29 11:35:17
// Desc:
//  New tcp connection.
//  ref muduo.

#pragma once

#include <Callbacks.h>
#include <InetAddress.h>

#include <memory>
#include <string>

namespace hquin {

class EventLoop;
class Channel;

// FIXME, use std::shared_from_this
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
  public:
    // Constructs a TcpConnection with a connected sockfd.
    TcpConnection(EventLoop *loop, std::string name, int sockfd,
                  const InetAddress &peerAddr);
    ~TcpConnection();

    const std::string &name() const { return name_; }
    const InetAddress &peerAddress() const { return peerAddr_; }
    bool connected() const { return state_ == kConnected; }

    void setConnectionCallback(const ConnectionCallback &cb) {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback &cb) {
        messageCallback_ = cb;
    }

    void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }

    // called when TcpServer accept a new connection.
    void connectEstablished();

    void connectDestroyed();

  private:
    enum StateE { kConnecting, kConnected, kDisconnected };

    void setState(StateE state) { state_ = state; }
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    EventLoop *eventloop_;
    std::string name_;
    StateE state_; // FIXME, use atomic variable?
    std::unique_ptr<Channel> channel_;
    InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
};

} // namespace hquin

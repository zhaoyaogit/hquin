// File:    TcpServer.h
// Author:  definezxh@163.com
// Date:    2019/06/24 10:18:21
// Desc:
//   Manage the TcpConnction obtained by accept(2).

#pragma once

#include <InetAddress.h>
#include <Callbacks.h>
#include <TcpConnection.h>

#include <functional>
#include <memory>
#include <map>
#include <string>

namespace hquin {

class EventLoop;
class Acceptor;

class TcpServer {
  public:
    TcpServer(EventLoop *loop, const InetAddress &listenAddr);
    TcpServer(const TcpServer &) = delete;
    TcpServer &operator=(const TcpServer &) = delete;
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback &cb) {
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback &cb) {
        messageCallback_ = cb;
    }

  private:
    // in event loop.
    void newConnection(int sockfd, const InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);

    typedef std::map<std::string, TcpConnectionPtr> ConncetionMap;

    EventLoop *eventloop_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    bool start_;
    int nextConnId_;
    ConncetionMap connections_;
};

} // namespace hquin

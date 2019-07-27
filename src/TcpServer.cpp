// File:    TcpServer.cpp
// Author:  definezxh@163.com
// Date:    2019/06/24 13:59:43
// Desc:
//   Manage the TcpConnction obtained by accept(2).

#include <TcpServer.h>
#include <EventLoop.h>
#include <Acceptor.h>
#include <TcpConnection.h>
#include <Log.h>
#include <sys/socket.h>

#include <stdio.h>
#include <assert.h>

using namespace std::placeholders;

namespace hquin {

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr)
    : eventloop_(loop), name_(listenAddr.stringifyHost()),
      acceptor_(std::make_unique<Acceptor>(eventloop_, listenAddr)),
      start_(false), nextConnId_(1) {
    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {}

void TcpServer::start() { acceptor_->listen(); }

// TcpServer establish a new connection(TcpConnection) When Acceptor accept(2) a
// new connction.
// peerAddr is the new connection InetAddress.
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    TcpConnectionPtr conn =
        std::make_shared<TcpConnection>(eventloop_, connName, sockfd, peerAddr);

    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);

    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
    conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    size_t n = connections_.erase(conn->name());
    LOG_INFO << "TcpServer::removeConnection [" << name_ << "] - connection "
             << conn->name();
    assert(n == 1);
    eventloop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

} // namespace hquin

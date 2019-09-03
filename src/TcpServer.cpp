// File:    TcpServer.cpp
// Author:  definezxh@163.com
// Date:    2019/06/24 13:59:43
// Desc:
//   Manage the TcpConnction obtained by accept(2).

#include <TcpServer.h>
#include <EventLoop.h>
#include <EventLoopThreadPool.h>
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
      threadPool_(std::make_unique<EventLoopThreadPool>(eventloop_)),
      start_(false), nextConnId_(1) {
    acceptor_->setNewConnectionCallback(
        [&](int sockfd, const InetAddress &peerAddr) {
            newConnection(sockfd, peerAddr);
        });
}

TcpServer::~TcpServer() {}

void TcpServer::start() {
    threadPool_->start();

    assert(!acceptor_->listenning());
    eventloop_->runInLoop([=]() { acceptor_->listen(); });
}

void TcpServer::setThreadNum(int numThreads) {
    assert(numThreads >= 0);
    threadPool_->setThreadNum(numThreads);
}

// TcpServer establish a new connection(TcpConnection) When Acceptor accept(2) a
// new connction.
// peerAddr is the new connection InetAddress.
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    eventloop_->assertInLoopThread();
    EventLoop *ioLoop = threadPool_->getNextLoop();

    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    TcpConnectionPtr conn =
        std::make_shared<TcpConnection>(ioLoop, connName, sockfd, peerAddr);

    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);

    conn->setCloseCallback(
        [&](const TcpConnectionPtr &conn) { removeConnection(conn); });
    ioLoop->runInLoop([=]() { conn->connectEstablished(); });
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    size_t n = connections_.erase(conn->name());
    assert(n == 1);

    EventLoop *ioLoop = conn->getLoop();
    // lambda captrue conn as value.
    ioLoop->queueInLoop([=]() { conn->connectDestroyed(); });
}

} // namespace hquin

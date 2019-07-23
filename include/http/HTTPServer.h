// File:    HTTPServer.h
// Author:  definezxh@163.com
// Date:    2019/07/23 17:40:00
// Desc:
//   HTTP Server, like tests/tcpserver_send_test

#pragma once

#include <http/HTTPRequest.h>
#include <http/HTTPResponse.h>
#include <TcpServer.h>

namespace hquin {

class EventLoop;

namespace http {

class HTTPServer {
  public:
    typedef std::function<void(const HTTPRequest &, HTTPResponse *)>
        HTTPCallback;

    HTTPServer(EventLoop *loop, InetAddress address);

    void setHTTPCallback(const HTTPCallback &cb) { httpCallback_ = cb; }

    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf,
                   Timestap receiveTime);
    void onRequest(const TcpConnectionPtr &conn, const HTTPRequest &req);

    void start();

  private:
    EventLoop *eventloop_;
    TcpServer server_;
    HTTPCallback httpCallback_;
};

} // namespace http

} // namespace hquin

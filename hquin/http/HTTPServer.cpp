// File:    HTTPServer.cpp
// Author:  definezxh@163.com
// Date:    2019/07/23 19:28:49
// Desc:
//   HTTP Server, like tests/tcpserver_send_test

#include <http/HTTPServer.h>
#include <Buffer.h>
#include <lim/Log.h>
#include <InetAddress.h>
#include <EventLoop.h>

using namespace std::placeholders;

namespace hquin {
namespace http {

HTTPServer::HTTPServer(EventLoop *loop, InetAddress address)
    : eventloop_(loop), server_(eventloop_, address), numThread_(0) {
    server_.setConnectionCallback(
        [&](const TcpConnectionPtr &conn) { onConnection(conn); });
    server_.setMessageCallback(
        [&](const TcpConnectionPtr &conn, Buffer *buffer,
            Timestamp receiveTime) { onMessage(conn, buffer, receiveTime); });
}

void HTTPServer::start() {
    LOG_INFO << "HTTPServer[" << server_.name() << "]";
    server_.setThreadNum(numThread_);
    server_.start();
}

void HTTPServer::onConnection(const TcpConnectionPtr &conn) {
    LOG_INFO << "new HTTP Request " << conn->peerAddress().stringifyHost();
}

void HTTPServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf,
                           Timestamp receiveTime) {
    HTTPRequest req(receiveTime);
    // TcpConnection buffer must update readable bytes.
    req.handle(buf->fetchReadble());
    onRequest(conn, req);
}

void HTTPServer::onRequest(const TcpConnectionPtr &conn,
                           const HTTPRequest &req) {
    const std::string &connection = req.getHeader("Connection");
    bool close =
        connection == "close" ||
        (req.version() == HTTPRequest::kHTTP10 && connection != "Keep-Alive");
    HTTPResponse response(close);
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(buf.stringifyReadable());
    buf.retrieveAll();
    if (response.closeConnection())
        conn->shutdown();
}

void HTTPServer::setThreadNum(int numThread) { numThread_ = numThread; }

} // namespace http
} // namespace hquin

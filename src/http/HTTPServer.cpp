// File:    HTTPServer.cpp
// Author:  definezxh@163.com
// Date:    2019/07/23 19:28:49
// Desc:
//   HTTP Server, like tests/tcpserver_send_test

#include <http/HTTPServer.h>
#include <Buffer.h>
#include <Log.h>
#include <InetAddress.h>
#include <EventLoop.h>

using namespace std::placeholders;

namespace hquin {
namespace http {

HTTPServer::HTTPServer(EventLoop *loop, InetAddress address)
    : eventloop_(loop), server_(eventloop_, address) {
    server_.setConnectionCallback(
        std::bind(&HTTPServer::onConnection, this, _1));
    server_.setMessageCallback(
        std::bind(&HTTPServer::onMessage, this, _1, _2, _3));
}

void HTTPServer::start() {
    LOG_WARN << "HttpServer[" << server_.name() << "]";
    server_.start();
}

void HTTPServer::onConnection(const TcpConnectionPtr &conn) {}

void HTTPServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf,
                           Timestap receiveTime) {
    HTTPRequest req(receiveTime);
    req.handle(buf->stringifyReadable());
    onRequest(conn, req);
}

void HTTPServer::onRequest(const TcpConnectionPtr &conn, const HTTPRequest &req) {
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

} // namespace http
} // namespace hquin
// File:    tcpserver_test.cpp
// Author:  definezxh@163.com
// Date:    2019/06/24 14:45:46
// Desc:
//   classes TcpServer test.

#include <TcpServer.h>
#include <InetAddress.h>
#include <EventLoop.h>

#include <iostream>

using namespace hquin;

void onConnection(const TcpConnectionPtr &conn) {
    if (conn->connected()) {
        printf("onConnection(): new connection [%s] from %s\n",
               conn->name().c_str(),
               conn->peerAddress().stringifyHost().c_str());
    } else {
        printf("onConnection(): connection [%s] is down\n",
               conn->name().c_str());
    }
}

void onMessage(const TcpConnectionPtr &conn, const char *data, ssize_t len) {
    printf("onMessage(): received %zd bytes from connection [%s]\n", len,
           conn->name().c_str());
}

int main() {
    EventLoop loop(32);
    InetAddress address(8002);
    TcpServer server(&loop, address);

    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();
    loop.loop();
}
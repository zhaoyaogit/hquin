// File:    tcpserver_send_test.cpp
// Author:  definezxh@163.com
// Date:    2019/07/08 16:30:46
// Desc:
//   send message test.

#include <TcpServer.h>
#include <InetAddress.h>
#include <EventLoop.h>
#include <Buffer.h>

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

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestap timestap) {
    printf("onMessage(): received %zd bytes from connection [%s]\n",
           buf->readableBytes(), conn->name().c_str());
    printf("[%s] receive data: %s\n", timestap.formatTimestap().c_str(),
           buf->stringifyReadable().c_str());
    conn->send(buf->fetchReadble());
}

int main(int argc, char *argv[]) {
    int port = atoi(argv[1]);

    EventLoop loop(32);
    InetAddress address(port);
    TcpServer server(&loop, address);

    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();
    loop.loop();
}
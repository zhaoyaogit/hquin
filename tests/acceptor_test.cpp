// File:    acceptor_test.cpp
// Author:  definezxh@163.com
// Date:    2019/06/19 22:31:18
// Desc:
//   test acceptor handle new connection.

#include <Acceptor.h>
#include <EventLoop.h>
#include <InetAddress.h>

#include <iostream>

using namespace hquin;

EventLoop *ploop;

void newConnection(int fd, const InetAddress &addr) {
    struct sockaddr_in clientaddr;
    uint32_t len;
    ::getpeername(fd, (struct sockaddr *)&clientaddr, &len);
    InetAddress address(clientaddr);
    address.setSockAddrInet(clientaddr);
    std::cout << address.stringifyHost() << std::endl;
}

int main() {
    EventLoop loop(128);
    ploop = &loop;
    InetAddress address(8086);
    Acceptor acceptor(ploop, address);
    acceptor.listen();
    acceptor.setNewConnectionCallback(newConnection);
    ploop->loop();
}
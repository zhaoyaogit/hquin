// File:    acceptor_test.cpp
// Author:  definezxh@163.com
// Date:    2019/06/19 22:31:18
// Desc:
//   get time routine of Echo client Ip and Address.
//   Now the server only have Acceptor classes.

#include <Acceptor.h>
#include <InetAddress.h>
#include <EventLoop.h>

#include <iostream>

using namespace hquin;

void getTime(int fd, const InetAddress &address) {
    InetAddress addr(address.getSockAddrInet());
    time_t ticks = time(NULL);
    std::cout << "Connect from " << addr.stringifyHost() << " at "
              << ctime(&ticks);
}

int main() {
    EventLoop loop(32);
    InetAddress address(8002);
    Acceptor acceptor(&loop, address);
    acceptor.setNewConnectionCallback(getTime);
    acceptor.listen();
    loop.loop();
}
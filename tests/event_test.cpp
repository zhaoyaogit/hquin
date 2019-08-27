// File:    EventLoop.cpp
// Author:  definezxh@163.com
// Date:    2019/04/29 20:57:57
// Desc:
//   test timerfd, stdin, stdout event.

#include <EventLoop.h>
#include <Channel.h>
#include <Timestamp.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/timerfd.h>
#include <string.h>
#include <errno.h>

#include <iostream>

using namespace hquin;

EventLoop *eventloop;

void hello(Timestamp timestamp) {
    std::cout << timestamp.formatTimestamp() << std::endl;
    eventloop->stopEvent();
    sleep(1);
}

int main() {
    EventLoop loop(1024);
    eventloop = &loop;

    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(eventloop, timerfd);
    channel.setReadCallback(hello);
    channel.enableReading();

    struct itimerspec howlong;
    memset(&howlong, 0, sizeof howlong);
    howlong.it_value.tv_sec = 1;
    timerfd_settime(timerfd, 0, &howlong, NULL);

    Channel channel2(eventloop, STDOUT_FILENO);
    channel2.setReadCallback(hello);
    channel2.enableReading();

    Channel channel3(eventloop, STDIN_FILENO);
    channel3.setReadCallback(hello);
    channel3.enableReading();

    eventloop->loop();
}

// File:    EventLoop.cpp
// Author:  definezxh@163.com
// Date:    2019/04/29 20:03:38

#include <EventLoop.h>

namespace hquin {

EventLoop::EventLoop() : stop_(false) {}

EventLoop::~EventLoop() {}

void EventLoop::loop() {
    while (!stop_)
        callback();
}

} // namespace hquin

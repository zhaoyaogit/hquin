// File:    EventLoop.cpp
// Author:  definezxh@163.com
// Date:    2019/04/29 20:57:57

#include <EventLoop.h>

#include <iostream>

using namespace hquin;

EventLoop el;

void hello() {
    std::cout << "hello\n";
    el.stopEvent();
}

int main() {
    el.setCallback(hello);
    el.loop();
}
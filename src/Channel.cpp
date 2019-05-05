// File:    Channel.cpp
// Author:  definezxh@163.com
// Date:    2019/04/30 16:06:25
// Desc:
//   Unified abstraction of all kinds IO with channel, containing fd and
//   corresponding event.
//   EventLoop only need to deal with channel whether it is file or socket.

#include <Channel.h>
#include <EventLoop.h>

#include <unistd.h>

namespace hquin {

Channel::Channel(EventLoop *eventloop, int fd)
    : eventloop_(eventloop), fd_(fd), mask_(NON_EVENT), epevent_{0} {
    epevent_.data.fd = fd_;
}

Channel::~Channel() { close(fd_); }

void Channel::enableReadable() {
    epevent_.events |= EPOLLIN;
    update();
}

void Channel::enableWritable() {
    epevent_.events |= EPOLLOUT;
    update();
}

// The channle's event corresponding callback function.
void Channel::handleEvent() {
    if (mask_ == READABLE_EVENT)
        readCallback_(fd_);
    else if (mask_ == WRITABLE_EVENT)
        writeCallback_(fd_);
}

// Update the channel's event through eventloop by epoll_ctl(2) do it actually.
void Channel::update() { eventloop_->updateChannel(this); }

} // namespace hquin

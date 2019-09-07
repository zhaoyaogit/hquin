// File:    Channel.cpp
// Author:  definezxh@163.com
// Date:    2019/04/30 16:06:25
// Desc:
//   Unified abstraction of all kinds IO with channel, containing fd and
//   corresponding event.
//   EventLoop only need to deal with channel whether it is file or socket.

#include <Channel.h>
#include <EventLoop.h>
#include <Timestamp.h>

#include <unistd.h>

namespace hquin {

const uint32_t Channel::kNonEvent = 0;
const uint32_t Channel::kReadEvent = EPOLLIN;
const uint32_t Channel::kWriteEvent = EPOLLOUT;
const uint32_t Channel::kErrorEvent = EPOLLERR | EPOLLHUP;

Channel::Channel(EventLoop *eventloop, int fd)
    : eventloop_(eventloop), fd_(fd), event_{0, {0}} {
    event_.data.fd = fd_;
}

Channel::~Channel() { close(fd_); }

void Channel::enableReading() {
    event_.events |= kReadEvent;
    update();
}

void Channel::enableWriting() {
    event_.events |= kWriteEvent;
    update();
}

void Channel::disableReading() {
    event_.events &= ~kReadEvent;
    update();
}

void Channel::disableWriting() {
    event_.events &= ~kWriteEvent;
    update();
}

void Channel::disableAll() {
    event_.events = kNonEvent;
    update();
}

// The channle's event corresponding callback function.
void Channel::handleEvent(Timestamp receiveTime) {
    if (event_.events & kErrorEvent) {
        closeCallback_(fd_);
    }

    if (event_.events & kReadEvent)
        readCallback_(receiveTime);

    if (event_.events & kWriteEvent)
        writeCallback_(fd_);
}

// Update the channel's event through eventloop by epoll_ctl(2) do it actually.
void Channel::update() { eventloop_->updateChannel(this); }

} // namespace hquin

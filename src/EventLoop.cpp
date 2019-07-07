// File:    EventLoop.cpp
// Author:  definezxh@163.com
// Date:    2019/04/29 20:03:38
// Desc:
//   class EventLoop is the eventloop program core by connecting Epoller and
//   Channel. A thread have a EventLoop only. Every event contain a member
//   EventLoop in order to update event itself in Epoller by EventLoop.

#include <EventLoop.h>
#include <Channel.h>
#include <Epoller.h>
#include <Timestap.h>

namespace hquin {

EventLoop::EventLoop(size_t size)
    : stop_(false), size_(size), epoller_(std::make_unique<Epoller>()) {}

EventLoop::~EventLoop() {}

// 1. classify event kinds.
// 2. time event
// 3. multiplexing IO
// 4. do for each ready event
// 5. callback();
void EventLoop::loop() {
    while (!stop_) {
        firedChannelList_.clear();

        // record the timestap of new connection arrived.
        Timestap receiveTime = epoller_->epoll(this, firedChannelList_);

        for (Channel *channel : firedChannelList_) {
            if (!channel->isNonEvent())
                channel->handleEvent(receiveTime);
        }
    }
}

void EventLoop::updateChannel(Channel *channel) {
    epoller_->updateEvent(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    epoller_->removeEvent(channel);
}

} // namespace hquin

// File:    Epoller.cpp
// Author:  definezxh@163.com
// Date:    2019/04/30 14:40:37
// Desc:
//   Multiplexing IO, do dispatching actually.

#include <Channel.h>
#include <Epoller.h>
#include <EventLoop.h>
#include <Log.h>

#include <unistd.h>

#include <assert.h>

namespace hquin {

// create a epoll instance.
// epoll_create(size), size just greater than 0 for kernel.
Epoller::Epoller() : epfd_(::epoll_create(1024)), events_(kInitEventListSize) {
    if (epfd_ == -1) {
        // throw error
    }
}

Epoller::~Epoller() { ::close(epfd_); }

// update the channel's event to epoll instance.
void Epoller::updateEvent(Channel *channel) {
    int op = 0;
    struct epoll_event event = channel->getEvent();

    if (channelMap_.find(channel->fd()) == channelMap_.end()) {
        op = EPOLL_CTL_ADD;
        channelMap_.emplace(channel->fd(), channel);
    } else {
        op = EPOLL_CTL_MOD;
    }

    // FIXME, syscall error handle
    ::epoll_ctl(epfd_, op, channel->fd(), &event);
} // namespace hquin

void Epoller::removeEvent(Channel *channel) {
    struct epoll_event event = channel->getEvent();

    // remove a tcp connection fd in epoll event.
    size_t n = channelMap_.erase(channel->fd());
    assert(n == 1);

    // FIXME, syscall error handle
    ::epoll_ctl(epfd_, EPOLL_CTL_DEL, channel->fd(), &event);
}

void Epoller::fillFiredEvents(int numevents,
                              std::vector<Channel *> &firedChannelList) {
    if (numevents <= 0)
        return;

    for (int i = 0; i < numevents; ++i) {
        struct epoll_event event = events_[i];

        std::map<int, Channel *>::const_iterator ch =
            channelMap_.find(event.data.fd);
        if (ch != channelMap_.end()) {
            Channel *channel = ch->second;
            channel->setEvent(event);
            firedChannelList.push_back(channel);
        }
    }
}

Timestap Epoller::epoll(EventLoop *eventloop,
                        std::vector<Channel *> &firedChannelList) {
    // call epoll_wait(2) with block.
    int numevents = ::epoll_wait(epfd_, events_.data(),
                                 static_cast<int>(events_.size()), 10000);
    Timestap receiveTime = Timestap::now();

    fillFiredEvents(numevents, firedChannelList);

    return receiveTime;
}

} // namespace hquin

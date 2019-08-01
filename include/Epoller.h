// File:    Epoller.h
// Author:  definezxh@163.com
// Date:    2019/04/30 11:41:52
// Desc:
//   Multiplexing IO, do dispatching actually.

#pragma once

#include <Timestap.h>

#include <sys/epoll.h>

#include <map>
#include <vector>

namespace hquin {

class Channel;
class EventLoop;

class Epoller {
  public:
    Epoller();
    Epoller(const Epoller &) = delete;
    Epoller &operator=(const Epoller &) = delete;
    ~Epoller();

    // actual operation of update channel.
    void updateEvent(Channel *channel);

    // remove event and its owner Channel
    void removeEvent(Channel *channel);

    // fill the ready events to fired channel list.
    void fillFiredEvents(int numevents,
                         std::vector<Channel *> &firedChannelList);

    // epoll_wait(2), and return receive timestap of new connection arrived.
    Timestap epoll(EventLoop *eventloop,
                   std::vector<Channel *> &firedChannelList);

  private:
    static const int kInitEventListSize = 16;

    int epfd_;
    std::vector<struct epoll_event> events_;
    std::map<int, Channel *> channelMap_;
};

} // namespace hquin

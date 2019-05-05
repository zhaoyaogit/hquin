// File:    Epoll.h
// Author:  definezxh@163.com
// Date:    2019/04/30 11:41:52
// Desc:
//   Multiplexing IO, do dispatching actually.

#pragma once

#include <sys/epoll.h>

#include <map>
#include <memory>
#include <vector>

namespace hquin {

class Channel;
class EventLoop;

class Epoll {
  public:
    Epoll();
    Epoll(const Epoll &) = delete;
    Epoll &operator=(const Epoll &) = delete;
    ~Epoll();

    void updateEvent(Channel *channel);
    void fillFiredEvents(int numevents,
                         std::vector<Channel *> &firedChannelList);
    int epoll(EventLoop *eventloop, std::vector<Channel *> &firedChannelList);

  private:
    int epfd_;
    std::unique_ptr<struct epoll_event> epevents_;
    std::map<int, Channel *> channelMap_;
};

} // namespace hquin

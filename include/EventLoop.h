// File:    EventLoop.h
// Author:  definezxh@163.com
// Date:    2019/04/29 19:07:55
// Desc:
//   EventLoop is the eventloop program core by connecting Epoller and
//   Channel. A thread have a EventLoop only. Every event contain a member
//   EventLoop pointer in order to update event itself in Epoller by EventLoop.

#pragma once

#include <functional>
#include <memory>
#include <map>
#include <vector>

namespace hquin {

#define NON_EVENT 0
#define READABLE_EVENT 1
#define WRITABLE_EVENT 2

class Channel;
class Epoller;

class EventLoop {
  public:
    EventLoop(size_t size);
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;
    ~EventLoop();

    // EventLoop has no channel member.
    // update event status actually do by epoll_ctl(2).
    void updateChannel(Channel *channel);

    // remove channel(event)
    void removeChannel(Channel *channel);

    void stopEvent() { stop_ = true; }

    // loop event queue, execute the callback function by ordinal.
    // if an event takes too long, it will seriously affect other ready event.
    void loop();

    size_t size() const { return size_; }

  private:
    bool stop_;
    size_t size_; // max event num in this event loop.
    std::unique_ptr<Epoller> epoller_;
    std::vector<Channel *> firedChannelList_;
};

} // namespace hquin

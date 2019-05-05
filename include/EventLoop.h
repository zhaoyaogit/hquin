// File:    EventLoop.h
// Author:  definezxh@163.com
// Date:    2019/04/29 19:07:55
// Desc:
//   class EventLoop is the eventloop program core by connecting Epoll and
//   Channel. A thread have a EventLoop only. Every event contain a member
//   EventLoop in order to update event itself in Epoll by EventLoop.

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
class Epoll;

class EventLoop {
  public:
    typedef std::function<void(void)> EventCallback;

    EventLoop(size_t size);
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;
    ~EventLoop();

    void updateChannel(Channel *channel);
    void stopEvent() { stop_ = true; }
    void loop();

    size_t size() const { return size_; }

  private:
    bool stop_;
    size_t size_; // max event num in this event loop.
    std::unique_ptr<Epoll> epoll_;
    std::vector<Channel *> firedChannelList_;
};

} // namespace hquin

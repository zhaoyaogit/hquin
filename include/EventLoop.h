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

class Channel;
class Epoller;

class EventLoop {
  public:
    typedef std::function<void()> Functor;

    EventLoop(size_t size);
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;
    ~EventLoop();

    size_t size() const { return size_; }

    void stopEvent() { stop_ = true; }

    // loop event queue, execute the callback function by ordinal.
    // if an event takes too long, it will seriously affect other ready event.
    void loop();

    // run callback immediately in the loop thread.
    void runInLoop(const Functor &func);

    // queues callback in the loop thread.
    // delay the execution time of pending functors to the next loop.
    // keep functors lifetime is same.
    // eg. if a close callback exec before read, the read callback must generate
    //   a error and dump.
    void queueLoop(const Functor &func);

    // execute pending functions in next event loop.
    void doPendingFunctors();

    // update event of channel in epoll fd.
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

  private:
    bool stop_;
    size_t size_; // max event num in this event loop.
    std::unique_ptr<Epoller> epoller_;
    std::vector<Channel *> firedChannelList_;
    std::vector<Functor> pendingFunctors_;
};

} // namespace hquin

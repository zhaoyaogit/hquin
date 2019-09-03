// File:    EventLoop.h
// Author:  definezxh@163.com
// Date:    2019/04/29 19:07:55
// Desc:
//   EventLoop is the eventloop program core by connecting Epoller and
//   Channel. A thread have a EventLoop only. Every event contain a member
//   EventLoop pointer in order to update event itself in Epoller by EventLoop.

#pragma once

#include <Util.h>

#include <sys/eventfd.h>

#include <functional>
#include <memory>
#include <map>
#include <vector>
#include <thread>
#include <mutex>

namespace hquin {

class Channel;
class Epoller;

class EventLoop {
  public:
    typedef std::function<void()> Functor;

    EventLoop();
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;
    ~EventLoop();

    void stop() { stop_ = true; }

    // loop event queue, execute the callback function by ordinal.
    // if an event takes too long, it will seriously affect other ready event.
    void loop();

    // one loop one thread.
    void assertInLoopThread();

    // loop must run in one thread.
    void abortNotInLoopThread();

    bool isInLoopThread() const { return threadId_ == gettid(); }

    // run callback immediately in the loop thread.
    void runInLoop(const Functor &func);

    // queues callback in the loop thread.
    // delay the execution time of pending functors to the next loop.
    // keep functors lifetime is same.
    // eg. if a close callback exec before read, the read callback must generate
    //   a error and dump.
    void queueInLoop(const Functor &func);

    // execute pending functions in next event loop.
    void doPendingFunctors();

    // update event of channel in epoll fd.
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    // handle wake up eventfd
    void handleRead();
    void wakeup();

  private:
    bool stop_;                   // atomic
    bool looping_;                // atomic
    bool callingPendingFunctors_; // atomic
    int wakeupFd_;
    pid_t threadId_;
    std::unique_ptr<Channel> wakeupChannel_;
    std::unique_ptr<Epoller> epoller_;
    std::vector<Channel *> firedChannelList_;
    std::vector<Functor> pendingFunctors_;
    std::mutex mutex_;
};

} // namespace hquin

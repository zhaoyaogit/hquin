// File:    EventLoopThreadPool.h
// Author:  definezxh@163.com
// Date:    2019/09/03 11:35:42
// Desc:
//   EventLoop thread pool, pruduce io thread.

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace hquin {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool {
  public:
    EventLoopThreadPool(EventLoop *baseLoop);
    EventLoopThreadPool(const EventLoopThreadPool &) = delete;
    EventLoopThreadPool &operator=(const EventLoopThreadPool &) = delete;
    ~EventLoopThreadPool();

    bool started() const { return started_; }

    // round-robin
    EventLoop *getNextLoop();

    void setThreadNum(int numThreads) { numThreads_ = numThreads; }

    void start();

  private:
    EventLoop *baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
};

} // namespace hquin

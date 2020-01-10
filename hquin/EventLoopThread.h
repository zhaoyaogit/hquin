// File:    EventLoopThread.h
// Author:  definezxh@163.com
// Date:    2019/09/03 11:01:11
// Desc:
//   One loop per thread.

#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

namespace hquin {

class EventLoop;

class EventLoopThread {
  public:
    EventLoopThread();
    EventLoopThread(const EventLoopThread &) = delete;
    EventLoopThread &operator=(const EventLoopThread &) = delete;
    ~EventLoopThread();

    EventLoop *startLoop();

  private:
    void threadFunc();

    EventLoop *loop_;
    bool exiting_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

} // namespace hquin

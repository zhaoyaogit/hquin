// File:    EventLoopThread.h
// Author:  definezxh@163.com
// Date:    2019/09/03 11:01:11
// Desc:
//   One loop per thread.

#include <EventLoopThread.h>
#include <EventLoop.h>
#include <Log.h>

namespace hquin {

EventLoopThread::EventLoopThread()
    : loop_(nullptr), exiting_(false),
      thread_(&EventLoopThread::threadFunc, this) {}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    loop_->stop();
    thread_.join();
}

EventLoop *EventLoopThread::startLoop() {
    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!loop_)
            cond_.wait(lock);
        loop = loop_;
    }

    LOG_INFO << "IO Loop created at " << loop->threadId();

    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    loop.loop();
    std::lock_guard<std::mutex> lock(mutex_);
    loop_ = nullptr;
}

} // namespace hquin

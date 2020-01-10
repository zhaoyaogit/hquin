// File:    EventLoopThreadPool.cpp
// Author:  definezxh@163.com
// Date:    2019/09/03 11:48:43
// Desc:
//   EventLoop thread pool, pruduce io thread.

#include <EventLoopThreadPool.h>
#include <EventLoop.h>
#include <EventLoopThread.h>

#include <assert.h>
#include <stdio.h>

namespace hquin {

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop)
    : baseLoop_(baseLoop), started_(false), numThreads_(0), next_(0) {}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start() {
    assert(!started_);
    baseLoop_->assertInLoopThread();

    started_ = true;

    for (int i = 0; i < numThreads_; ++i) {
        std::unique_ptr<EventLoopThread> t =
            std::make_unique<EventLoopThread>();
        loops_.push_back(t->startLoop());
        threads_.push_back(std::move(t)); // the move step must be at the end.
    }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    assert(started_);

    EventLoop *loop = baseLoop_;
    if (!loops_.empty()) {
        loop = loops_[next_];
        ++next_;
        next_ %= loops_.size();
    }

    return loop;
}

} // namespace hquin

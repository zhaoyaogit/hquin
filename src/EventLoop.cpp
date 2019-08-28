// File:    EventLoop.cpp
// Author:  definezxh@163.com
// Date:    2019/04/29 20:03:38
// Desc:
//   class EventLoop is the eventloop program core by connecting Epoller and
//   Channel. A thread have a EventLoop only. Every event contain a member
//   EventLoop in order to update event itself in Epoller by EventLoop.

#include <EventLoop.h>
#include <Channel.h>
#include <Epoller.h>
#include <Timestamp.h>
#include <Log.h>

#include <unistd.h>

#include <assert.h>

namespace hquin {

__thread EventLoop *t_loopInThisThread = 0;

EventLoop::EventLoop(size_t size)
    : stop_(false), looping_(false), callingPendingFunctors_(false),
      size_(size), wakeupFd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
      threadId_(gettid()),
      wakeupChannel_(std::make_unique<Channel>(this, wakeupFd_)),
      epoller_(std::make_unique<Epoller>()) {
    if (wakeupFd_ < 0) {
        LOG_ERROR << "Failed in eventfd";
    } else {
        wakeupChannel_->setReadCallback(
            std::bind(&EventLoop::handleRead, this));
        wakeupChannel_->enableReading();
    }

    if (t_loopInThisThread) {
        LOG_ERROR << "Another EventLoop "
                  << *reinterpret_cast<size_t *>(t_loopInThisThread)
                  << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = NULL;
}

// loop the event
void EventLoop::loop() {
    assert(!looping_);
    looping_ = true;
    stop_ = false;

    while (!stop_) {
        firedChannelList_.clear();

        // record the timestamp of new connection arrived.
        Timestamp receiveTime = epoller_->epoll(this, firedChannelList_);

        for (Channel *channel : firedChannelList_) {
            if (!channel->isNonEvent())
                channel->handleEvent(receiveTime);
        }

        doPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::assertInLoopThread() {
    if (!isInLoopThread())
        abortNotInLoopThread();
}

void EventLoop::abortNotInLoopThread() {
    pid_t threadId = gettid();
    LOG_ERROR << "EventLoop::abortNotInLoopThread - EventLoop "
              << *reinterpret_cast<size_t *>(this)
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << threadId;
}

void EventLoop::runInLoop(const Functor &func) {
    // in current thread exec callback straightly.
    if (isInLoopThread())
        func();
    else
        queueInLoop(func);
}

void EventLoop::queueInLoop(const Functor &func) {
    {
        std::lock_guard<std::mutex> guard(mutex_);
        pendingFunctors_.push_back(func);
    }

    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->ownLoop() == this);
    assertInLoopThread();
    epoller_->updateEvent(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    epoller_->removeEvent(channel);
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR << "EventLoop::handleRead() reads " << n
                  << " bytes instead of 8";
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR << "EventLoop::wakeup() writes " << n
                  << " bytes instead of 8";
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::lock_guard<std::mutex> guard(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (Functor func : functors)
        func();

    callingPendingFunctors_ = false;
}

} // namespace hquin

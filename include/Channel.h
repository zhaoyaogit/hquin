// File:    Channel.h
// Author:  definezxh@163.com
// Date:    2019/04/30 15:52:52
// Desc:
//   Unified abstraction of all kinds IO with channel, containing fd and
//   corresponding event.
//   EventLoop only need to deal with channel whether it is file or socket.

#pragma once

#include <Timestap.h>

#include <sys/epoll.h>

#include <functional>

namespace hquin {

class EventLoop;

class Channel {
  public:
    typedef std::function<void(int)> EventCallback;
    typedef std::function<void(Timestap)> ReadEventCallback;
    Channel(EventLoop *eventloop, int fd);
    Channel(const Channel &) = delete;
    Channel &operator=(const Channel &) = delete;
    ~Channel();

    // update the event.
    void enableReadable();
    void enableWritable();
    void disableAll();

    void setReadCallback(const ReadEventCallback &cb) { readCallback_ = cb; }
    void setWriteCallback(const EventCallback &cb) { writeCallback_ = cb; }
    void setErrorCallback(const EventCallback &cb) { errorCallback_ = cb; }
    void setCloseCallback(const EventCallback &cb) { closeCallback_ = cb; }

    // execute callback function depends on event type.
    void handleEvent(Timestap receiveTime);

    int fd() const { return fd_; }

    void setEvent(const struct epoll_event &event) { event_ = event; }
    struct epoll_event getEvent() const {
        return event_;
    }

    // event type serial.
    bool isNonEvent() const { return event_.events == kNonEvent; }
    bool isWriting() const { return event_.events & kWriteEvent; }

  private:
    // update itself to event loop.
    void update();

    static const uint32_t kNonEvent;
    static const uint32_t kReadEvent;
    static const uint32_t kWriteEvent;

    EventLoop *eventloop_;
    int fd_;
    struct epoll_event event_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};

} // namespace hquin

// File:    Channel.h
// Author:  definezxh@163.com
// Date:    2019/04/30 15:52:52
// Desc:
//   Unified abstraction of all kinds IO with channel, containing fd and
//   corresponding event.
//   EventLoop only need to deal with channel whether it is file or socket.

#pragma once

#include <sys/epoll.h>

#include <functional>

namespace hquin {

class EventLoop;

class Channel {
  public:
    typedef std::function<void(int)> EventCallback;
    Channel(EventLoop *eventloop, int fd);
    Channel(const Channel &) = delete;
    Channel &operator=(const Channel &) = delete;
    ~Channel();

    // FIXME: mask and event error
    void enableReadable();
    void enableWritable();
    void disableAll() {}
  
    void setReadCallback(const EventCallback &cb) { readCallback_ = cb; }
    void setWriteCallback(const EventCallback &cb) { writeCallback_ = cb; }
    void setErrorCallback(const EventCallback &cb) { errorCallback_ = cb; }
    void setCloseCallback(const EventCallback &cb) { closeCallback_ = cb; }

    // execute callback function depends on
    // mask {NON_EVENT | READABLE_EVENT |WRITABLE_EVENT}
    void handleEvent();

    // update itself to events queue by eventloop point.
    void update();

    int fd() const { return fd_; }
    int mask() const { return mask_; }
    void setMask(int mask) { mask_ = mask; }
    struct epoll_event event() {
        return event_;
    }

  private:
    EventLoop *eventloop_;

    int fd_;
    int mask_; // {NON_EVENT | READABLE_EVENT | WRITABLE_EVENT}
    struct epoll_event event_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};

} // namespace hquin

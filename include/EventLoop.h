// File:    EventLoop.h
// Author:  definezxh@163.com
// Date:    2019/04/29 19:07:55

#pragma once

#include <functional>

namespace hquin {

class EventLoop {
  public:
    typedef std::function<void(void)> EventCallback;

    EventLoop();
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;
    ~EventLoop();

    void setCallback(const EventCallback &cb) { callback = cb; }
    void stopEvent() { stop_ = true; }
    void loop();

  private:
    EventCallback callback;

    bool stop_;
};

} // namespace hquin

// File:    Acceptor.h
// Author:  definezxh@163.com
// Date:    2019/06/19 11:37:51
// Desc:
//   Acceptor corresponding to each connection.

#pragma once

#include <Channel.h>
#include <InetAddress.h>

#include <functional>
#include <memory>

namespace hquin {

class EventLoop;

class Acceptor {
  public:
    typedef std::function<void(int fd, const InetAddress &)>
        NewConnectionCallback;

    Acceptor(EventLoop *eventloop, const InetAddress &addr);
    Acceptor(const Acceptor &) = delete;
    Acceptor &operator=(const Acceptor &) = delete;
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb) {
        newConnectionCallback_ = cb;
    }

    bool listenning() const { return listenning_; }

    // listen() could execute on constructor.
    // now it is flexible as a member function.
    void listen();

  private:
    void handleRead();

    EventLoop *eventloop_;
    int sockfd_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    InetAddress servaddr_;
    bool listenning_;
};

} // namespace hquin

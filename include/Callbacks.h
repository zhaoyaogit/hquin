// File:    Callbacks.h
// Author:  definezxh@163.com
// Date:    2019/06/28 22:14:33
// Desc:
//   callback

#pragma once

#include <Timestamp.h>

#include <functional>
#include <memory>

namespace hquin {

class TcpConnection;
class Buffer;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;

typedef std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)>
    MessageCallback;

typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;

} // namespace hquin

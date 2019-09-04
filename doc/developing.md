### 一些开发时的问题思考

***很大程度的参考了 [Linux 多线程服务端编程：使用 muduo C++ 网络库](http://chenshuo.com/book/)***

- [类应为单向依赖](#one_way_dep)
- [关于类中的std::bind()](#bind)
- [对类的封装的思考](#wrapper)
- [TcpConnection 生命周期的坑](#shard_ptr)
- [将 std::bind() 替换为lambda](#bind2lambda)

#### <span id = "one_way_dep"> 类应为单向依赖 </span>
单向依赖，可以屏蔽依赖类的细节，而依赖类则并不知道被依赖的存在，简化设计的流程。如 `Acceptor` 和 `TcpServer`， 对于 `TCPServer` 来说，并需要知道 `Acceptor` 的实现细节，只需要获得一个新的连接而已，而 `Acceptor` 也不清楚 `TCPServer` 的存在。
这样在实现的时候，先实现好 `Acceptor` 然后在 main 函数中直接调用让用户控制该对象的生命周期。在原型大致完备后又将 `Acceptor` 集成到 `TcpServer` 中，当 `TcpServer` 需要添加功能时，直接顺藤摸瓜，向其依赖类一路修改过去。

#### <span id = "bind"> 关于类中的std::bind() </span>
使用 使用bind函数将实际执行函数绑定到回调函数中，***hquin 中所有的回调函数都是可以设置的。***
在 事件循环中 分发给各个就绪的Channel执行函数为 `Channel::handleEvent()` ，通过这个函数，我们可以根据具体的状态分发实际执行的功能函数。
对于Channel而言，功能函数为 read, write 等回调函数，且可以通过 ***设置回调函数来改变执行的功能***

Channel的生命周期为其所有者控制。
1. 所有者`Acceptor`, 建立通信连接。在建立连接的时候绑定`Channel`的回调函数为 `Acceptor::handleRead()` 来获取已完成连接队列的队头项。而有趣的是 Acceptor 中也有回调函数，在 handleRead 中又执行该回调函数。
2. 所有者 `TcpConnection`, 处理新的连接。当需要处理新的连接时，需要创建一个 TcpConnection 对象，继续上面的操作，绑定`Channel`的回调函数为实际的处理新连接函数，
3. 新的连接是得知而建立的？ 在执行 `Acceptor::handleRead()` 会继续调用回调函数，Acceptor 的所有者为 TcpServer, 在构造TcpServer对象时就将创建TcpConnection 对象的函数设置为 Acceptor 的回调函数，这样就一路调用了。

有 **类应为单向依赖** 这个约定，这个调用的过程可以不用负担太多心智包袱。
`Acceptor::handleRead()` -> `Acceptor::callback()` -> `TcpServer::newTcpConnection()` -> `TcpConnection::callback()`

#### <span id = "wrapper"> 对类的封装的思考 </span>
对类的封装要考虑其通用性，如在刚开始时对 `struct sockaddr_in` 的简单粗暴的处理，将一切关于该结构的函数作为该类的成员函数。
```cpp
class InetAddress;
InetAddress addr(port);
addr.bindSockAddrInet(sockfd);
```
像上面我将 bind 函数做为该类的成员函数，而 bind(2) 时服务端调用的接口，在客户时不调用的，这就造成了逻辑上的混乱，在随后调用accept(2) 的时候
```cpp
InetAddress servaddr_; // Acceptor member variable

InetAddress newConnAddr(servaddr_);
int connfd = newConnAddr.acceptSockAddrInet(sockfd_);
```
当时写的时候也是很蛋疼的，以listen InetAddress 构造了一个新的连接地址，这看起来很令人费解。

后面将 bind 等socket函数从 `InetAddress` 类中删除，保留只与 `sockaddr_in` 结构信息相关的函数，而将 socket 系列函数封装进另外一个围绕sockfd封装其socket系列函数的类 `Socket`中。

#### <span id = "shared_ptr"> TcpConnection 生命周期的坑 </span>
因为存在各种回调函数和事件循环存在，TcpConnection 的生命周期不是固定的，于是使用 std::shared_from_this, 这样只要对 TcpConnection 对象还存在引用就不会被销毁。
后来总是出现 coredump，通过排查引用计数
```cpp
// 内存上多出一个对 该对象的应用 use_count() 就会 + 1.
// use_count() + 1 
TcpConnectionPtr conn =
    std::make_shared<TcpConnection>(ioLoop, connName, sockfd, peerAddr);
// use_count() + 1
connections_[connName] = conn;

// use_count() + 1
callback(); // 执行完后，自动 - 1

// 函数执行完成（或者说内存回收），引用数 -1
// use_count() - 1
size_t n = connections_.erase(conn->name());

// use_count() - 1, 实际上取消对该对象的引用是在 Epller::removeEvent() 中
ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
```
关闭连接从 map 中删除减少一次，如果关闭函数执行完成，改对象就会被析构。
但是代码的逻辑 处理可读函数在可读为0时会关闭此连接，而在事件循环中处理可写函数会继续执行，这样可写函数就会访问未知逻辑直接挂掉，所以这里推迟关闭函数的执行，使其在下一次事件循环中被调用。

还有一个问题，如果关闭函数不执行的话，对象会一直存在不被析构，文件描述符就不会被回收。

#### <span id = "bind2lambda"> 将std::bind()替换为lambda </span>
std::bind() 使用的一些想法继续保留在doc中。
使用 std::bind() 的可读性不是很友好，在C++ 14后可以完全用lambda代替std::bind()。替换的方式很简单，但是这个有一个非常需要谨慎的地方就是 TcpConnection 的生命周期的问题，
```cpp
ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
// 使用 lambda 替换 std::bind()
ioLoop->queueInLoop([=]() { conn->connectDestroyed(); });
```
在于捕获是使用值捕获还是引用捕获的问题，如果使用引用捕获，函数结束 TcpConnection的生命周期就结束，因为取的引用的引用并不能延长其生命周期。改成值捕获后增加 TcpConnectionPtr(std::shared_ptr) 的引用计数，使得TcpConnection 的生命周期不会立刻结束。
***Linux 下的多线程非阻塞 TCP Server.***


**最基础的事件驱动构件**
- `EventLoop`: 整个事件循环的核心，通过各个类中的 `EventLoop` 指针用 `pImpl` 手法把各功能模块之间的连接起来。包括，
	- `Channel*` 数组，包含活跃的事件对应的 `Channel`, std::vector 实现。
	- `Epoller`对象 的 unique 指针，控制 epoll(2) 的生命周期，本质是一个事件循环对于一个 epoll(2) 实例，当前 `EventLoop` 生命周期结束 `Epoller` 也自动销毁。

- `Channel`: 对一个文件IO的抽象，包括，
	- 文件描述符
	- 事件
	- 回调函数
	- 事件更新函数, `EventLoop` 指针的 pImpl 手法

- `Epoller`: 封装IO多路复用, 
	- `<fd, Channel*>` 的 std::map，将文件描述符和 `Channel*` 对应起来，在某个事件有更新的时更新其对应的 `Channel`

**以上三个类构成了简单的 Reactor 模型，作为整个 事件驱动 的基础，运行情况如下：**
1. `Channel` 先设置好事件及其对应的回调函数，通过 成员变量 `EventLoop` 指针将自身更新至每个 `EventLoop` 中的 `Epoller` 实例 <fd, Channel*> map 中
2. `EventLoop` 调用 `Epoller` 中的 IO多路复用函数，得到准备好了的事件（数组），并且将事件对应的 `Channel*` 添加到 `EventLoop` 中的活跃事件对应的 `Channel*` 数组中，执行各个 `Channel` 对应的事件回调函数。
3. `EventLoop` 循环执行完一轮循环检查 循环标志 判断是否继续执行，故需要退出的时间不能确定。

**TCPServer**
- `InetAddress`, 对IPv4的套接字地址封装
- `Acceptor`, 每个连接对应的接收器，将 accept 函数绑定到 Channel 的回调函数上。当有新连接到达时，通过多路复用回调 accept 函数，取出监听的套接字队列中已完成的连接。在程序启动前，先完成 socket(), bind(), listen(), 再等待 connect() 函数发送的连接。


**参考**
- [muduo](https://github.com/chenshuo/muduo), 参考最多的网络库，对类的设计非常精妙，Reactor 模式也是由 muduo 领入门的。
- [redis](https://github.com/antirez/redis), 主要参考 redis 事件驱动模块，ae_* 的几个文件。 
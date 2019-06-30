### Linux 下的多线程非阻塞 TCP Server.

定个小目标：可以处理 10K 数量的请求连接

##### TCPServer

一个简单的 Echo 服务器程序
```c
 13 int main(int argc, char *argv[]) {
 14     if (argc < 2)
 15         return 1;
 16 
 17     uint16_t port = (uint16_t)(atoi(argv[1]));
 18 
 19     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
 20 
 21     struct sockaddr_in servaddr;
 22 
 23     servaddr.sin_family = AF_INET;
 24     servaddr.sin_port = htons(port);
 25     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 26 
 27     bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
 28     listen(sockfd, SOMAXCONN);
 29 
 30     for ( ; ; ) {
 31         struct sockaddr_in connect_addr;
 32         socklen_t len = sizeof(connect_addr);
 33         int connfd = accept(sockfd, (struct sockaddr *)&connect_addr, &len);
 34 
 35         char buf[32];
 36         printf("connection fromm %s, port %d.\n",
 37                inet_ntop(AF_INET, &connect_addr.sin_addr, buf, sizeof(buf)),
 38                ntohs(connect_addr.sin_port));
 39 
 40         write(connfd, buf, strlen(buf));
 41         close(connfd);
 42     }
 43 }
```

以Echo为原型进行扩展

- L19 - L28 为 TCP 服务端为连接前创建的条件
	1. 指定期望的通信协议类型
	2. 为本地协议地址赋予一个套接字。
	3. 将主动套接字转换为被动套接字，对于监听套接字维护两个队列 [listen()](./doc/basic_socket.md)
- L30 - L41 为处理新连接的过程
	1. 从已完成队列中取出（accept）队头项，处理新的连接

前一个阶段为固定的步骤，这部分不需要做什么额外的处理，而集中处理的地方为第二部分，对于新连接的处理过程要考虑的
1. 如何处理多个并发请求，是使用多线程还是多进程
2. 处理请求是阻塞式的还是非阻塞的
3. 是否使用多路复用，使用多路复用是使用 select poll 还是 epoll
4. 是否支持日志记录


在解决以上几个问题前，先做一些准备操作
1. 封装 `struct sockaddr_in` 结构，对使用该结构的类进行屏蔽内部操作
2. 封装 `socket` 系列函数调用，屏蔽内部实现细节
3. 构建基础的事件循环的框架，使用多路复用来处理新的连接

##### 最基础的事件驱动构件
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

以上三个类构成了简单的 Reactor 模型，作为整个 事件驱动 的基础，运行情况如下：
1. `Channel` 先设置好事件及其对应的回调函数，通过 成员变量 `EventLoop` 指针将自身更新至每个 `EventLoop` 中的 `Epoller` 实例 <fd, Channel*> map 中
2. `EventLoop` 调用 `Epoller` 中的 IO多路复用函数，得到准备好了的事件（数组），并且将事件对应的 `Channel*` 添加到 `EventLoop` 中的活跃事件对应的 `Channel*` 数组中，执行各个 `Channel` 对应的事件回调函数。
3. `EventLoop` 循环执行完一轮循环检查 循环标志 判断是否继续执行，故需要退出的时间不能确定。


##### TODO
1. 使用多线程处理并发请求
2. 支持日志系统

##### DONE
1. 采用多路复用，因此必须是非阻塞的请求

##### [一些开发时的问题思考](./doc/developing.md)

**参考**
- [muduo](https://github.com/chenshuo/muduo), 参考最多的网络库，对类的设计非常精妙，Reactor 模式也是由 muduo 领入门的。
- [redis](https://github.com/antirez/redis), 主要参考 redis 事件驱动模块，ae_* 的几个文件。 
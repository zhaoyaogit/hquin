##### 值-结果 参数
当往一个套接字函数传递一个套接字地址结构时，该结构的长度传递方式取决于该结构的传递方向：
- 从进程至内核，仅需要告诉内核需要从进程内复制多少数据即可。
这样的函数有 `bind(2)`, `connect(2)`, `sendto(2)`
- 从内核至进程，这种类型的参数成为 `值-结果`, 为进程和内核之间相互传递长度。在函数返回时内核告诉进程内核在该结构内存储了多少信息。
从内核到进程的函数有4个：`accept(2)`, `revcfrom(2)`, `getsockname(2)`, `getpeername(2)`

#### 相关套接字 API

##### socket 函数
用于指定期望的通信协议类型，在进行网络IO时必须第一个调用的函数。

``` c
#include <sys/socket.h>
int socket(int family, int type, int protocol);
```

##### connect 函数
TCP 客户用 connect 函数来建立与TCP服务器的连接。
```c
#include <sys/socket.h>
int connect(int sockfd, const struct sockaddr *sevraddr, socklen_t addrlen);
```

对应TCP状态转换图, connect 函数导致当前套接字的从 `CLOSED` 状态（自socket函数创建来的状态）转移到 `SYS_SENT` 状态，若成功则转换为 `ESTABLISHED` 状态。该函数是 TCP 建立连接的三次握手过程。

##### bind 函数
bind 将本地协议地址赋予一个套接字。
```c
#include <sys/socket.h>
int bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);
```

- 端口绑定
一般而言，服务器会在启动的时候绑定一个端口。若未绑定固定的端口，在调用 connect 和 listen 函数时，内核会为相应的套接字选择一个临时的端口，而客户端一般就是这样做的。
- IP 地址绑定
对于服务器，限定了该套接字只接收这些目的地址为这个IP地址的客户连接。对于TCP客户，在该套接字上发送的IP数据报指定了源IP地址。
当客户不绑定IP地址时，内核将更具所用外出网络接口来选择源IP地址（这样就经常变动了）。
当服务器不绑定时，内核将客户发送的 `SYN` 的目的地址作为服务器的源地址。

##### listen 函数
仅用于TCP服务器调用，做两件事情
- listen 将未连接（主动）套接字转换成一个被动套接字，根据状态转换图为套接字从 `CLOSED` 转换成 `LISTEN`
- 函数的第二个参数规定了内核应该为相应套接字排队的最大连接个数
```c
#include <sys/socket.h>
int bind(int sockfd, int backlog);
```

内核会为任何一个给定的监听套接字维护两个队列
- 未完成队列，每个这样的SYN分节对应其中一项：
服务器端正在等待相应的 TCP 三次握手过程(等待 ACK)，这些套接字状态处于 `SYN_REVD`。队列中的任何一项存留的时间为一个 `RTT`
- 已完成队列，每个已完成三次握手过程的套接字，状态处于 `ESTABLISHED`

##### accept 函数
由 TCP服务端调用，用于从已完成连接队列头返回下一个已完成连接，若已完成队列为空，则进程被投入睡眠(阻塞方式)
```c
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *cleaddr, socklen_t *addrlen);
```
accept 成功，其返回值为一个由内核分配的全新描述符，代表与所返回客户端的TCP连接，服务器在完成给定用户的服务时，相应的已连接套接字就被关闭。
而函数第一个参数为监听套接字，在该服务器网络IO生命周期内一直存在。
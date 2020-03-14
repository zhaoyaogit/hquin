## hquin

![](https://www.travis-ci.org/zxhio/hquin.svg?branch=master)

hquin 是一个基于 Reactor 模式运行在 Linux 下采用 C++ 开发的 TcpServer。
- 异步日志
- Modern C++
- 多线程处理，one loop per thread + thread pool.

### 目录
- doc - 一些开发时的问题思考
- include/src - 代码文件
- tests - 关于hquin用法的一些测试程序

### 编译
```shell
$ make build && cd build
$ cmake .. && make
```

### 依赖
- [limlog](https://github.com/zxhio/limlog)

### TODO
完善测试用例

### 参考
[Muduo](https://github.com/chenshuo/muduo)，深度参考(copy)
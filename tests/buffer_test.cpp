// File:    buffer_test.cpp
// Author:  definezxh@163.com
// Date:    2019/07/02 19:37:42
// Desc:
//   Buffer test

#include <Buffer.h>

#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <string.h>

using namespace hquin;

int main() {
    int fd = open("/usr/include/syscall.h", O_RDONLY);
    Buffer buffer;
    int error;
    buffer.readFd(fd, &error);

    std::cout << buffer.stringifyReadable() << std::endl;
    std::cout << "readable bytes: " << buffer.readableBytes() << std::endl;
}
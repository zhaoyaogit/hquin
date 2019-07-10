// File:    Buffer.cpp
// Author:  definezxh@163.com
// Date:    2019/07/02 09:51:13
// Desc:
//   Use Buffer class instead of @c string. Implementing only functions without
//   considering performance for the time being.

#include <Buffer.h>
#include <Log.h>

#include <unistd.h>

#include <algorithm>

namespace hquin {

// change the buffer capacity or move data in itself buffer
void Buffer::modifySpace(size_t len) {
    if (len < writableBytes()) {
        // writable bytes is enough.
        // do nothing.
    } else if (writeIndex_ - readIndex_ + len < buffer_.capacity()) {
        // total free buffer is enough to write data. in order to reduce number
        // of resize, we could move data to the front of buffer.
        size_t readable = readableBytes();
        std::copy(beginRead(), beginWrite(), buffer_.data());
        writeIndex_ -= readIndex_;
        readIndex_ = 0;
        assert(readable == readableBytes()); // readable should not change.
    } else {
        buffer_.reserve(writeIndex_ + len);
    }
}

// append data to buffer end.
void Buffer::append(const std::string buf) { append(buf.data(), buf.length()); }

void Buffer::append(const char *buf, size_t len) {
    modifySpace(len);
    std::copy(buf, buf + len, beginWrite());
    writeIndex_ += len;
}

ssize_t Buffer::readFd(int fd, int *savedErrno) {
    char buf[2048];
    const ssize_t n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        *savedErrno = static_cast<int>(errno);
    } else {
        append(buf, n);
    }
    return n;
}

std::string Buffer::stringifyReadable() {
    std::string buf(beginRead(), readableBytes());
    return buf;
}

std::string Buffer::fetchReadble() {
    std::string str = stringifyReadable();
    retrieveAll();
    return str;
}

} // namespace hquin

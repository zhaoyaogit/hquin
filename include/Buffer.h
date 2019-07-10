// File:    Buffer.h
// Author:  definezxh@163.com
// Date:    2019/07/01 23:42:36
// Desc:
//   Use Buffer class instead of @c string. Implementing only functions without
//   considering performance for the time being.

#pragma once

#include <memory>
#include <vector>
#include <string>

#include <assert.h>
#include <stddef.h> // ssize_t

namespace hquin {

// Use interger index to control read/write position.
class Buffer {
  public:
    static const size_t kInitialSize = 1024;
    Buffer() : buffer_(kInitialSize), readIndex_(0), writeIndex_(0) {
        assert(readableBytes() == 0);
        assert(writableBytes() == kInitialSize);
    }

    size_t readableBytes() const { return writeIndex_ - readIndex_; }
    size_t writableBytes() const { return buffer_.size() - writeIndex_; }

    char *beginRead() { return buffer_.data() + readIndex_; }

    char *beginWrite() { return buffer_.data() + writeIndex_; }

    // modify buffer structure, reduce number of buffer resize.
    void modifySpace(size_t len);

    // append data to buffer end.
    void append(const std::string buf);
    void append(const char *buf, size_t len);

    // read data directly into buffer.
    // @return result of read(2), @c errno is saved
    ssize_t readFd(int fd, int *savedErrno);

    // change readIndex_
    void retrieve(size_t len) {
        assert(len <= readableBytes());
        readIndex_ += len;
    }

    // reset index.
    void retrieveAll() {
        readIndex_ = 0;
        writeIndex_ = 0;
    }

    // stringify readable data.
    std::string stringifyReadable();

    // stringify readble data and reset index.
    std::string fetchReadble();

  private:
    std::vector<char> buffer_; // let STL to handle memory.
    size_t readIndex_;
    size_t writeIndex_;
}; // namespace hquin

} // namespace hquin

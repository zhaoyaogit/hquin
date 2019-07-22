// File:    Queue.h
// Author:  definezxh@163.com
// Date:    2019/07/22 16:10:23
// Desc:
//   Multi-Producer and Single-Customer ring queue.

#pragma once

#include <memory>

namespace hquin {

static inline bool isPowerOf2(size_t n) { return n && !(n & (n - 1)); }

template <typename T> class RingQueue {
  public:
    RingQueue(size_t n)
        : readIndex_(0), writeIndex_(0), size_(n),
          data_(std::make_unique<T[]>(size_)) {}

    bool isEmpty() const { return writeIndex_ == readIndex_; }
    bool isFull() const { return writeIndex_ - readIndex_ == size_; }

    void push(const T &item) { data_[writeIndex_++ % size_] = item; }
    void push(T &&item) { data_[writeIndex_++ % size_] = std::move(item); }
    T pop() { return data_[readIndex_++ % size_]; }
    void pop(T &item) { item = std::move(data_[readIndex_++ % size_]); }
    T front() { return data_[readIndex_]; }

  private:
    size_t readIndex_;
    size_t writeIndex_;
    size_t size_;
    std::unique_ptr<T[]> data_;
};

} // namespace hquin

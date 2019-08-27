// File:    Log.h
// Author:  definezxh@163.com
// Date:    2019/07/03 20:31:34
// Desc:
//   A simple logger. only support log to file.
//   File format default is log20190805_x.log, x is roll file number.

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <mutex>
#include <thread>

#include <sys/epoll.h>
#include <sys/eventfd.h>

#include <string.h>
#include <stdio.h>

namespace hquin {

// Fixed buffer, is same as std::array but its memory alloced at heap.
class FixedBuffer {
  public:
    explicit FixedBuffer(size_t size);

    size_t size() const { return usedBytes_; }
    size_t capacity() const { return bufferSize_; }

    bool isEmpty() const { return usedBytes_ == 0; }
    bool isFull() const { return usedBytes_ == bufferSize_; }

    // used bytes reset 0.
    void reset();

    void bzero();

    // buffer begin address.
    char *begin() { return &buffer_.get()[0]; }

    // buffer begin append address.
    char *buffer() { return &buffer_.get()[usedBytes_]; }

    // append data to buffer.
    void append(const char *str, size_t len); // append @c string.
    void append(const char *str) { append(str, strlen(str)); }

  private:
    size_t usedBytes_;
    size_t bufferSize_;
    std::unique_ptr<char[]> buffer_;
};

template <size_t N> class SmartBuffer {
  public:
    SmartBuffer();

    size_t size() const { return usedBytes_; }
    size_t capacity() const { return bufferSize_; }

    char *begin() { return heapBuffer_ ? heapBuffer_->begin() : stackBuffer_; }
    char *buffer() { // get first unused bytes address.
        return heapBuffer_ ? heapBuffer_->buffer() : &stackBuffer_[usedBytes_];
    }

    void append(const char *str, size_t len); // append @c string.
    void append(const char *str) { append(str, strlen(str)); }

  private:
    void resizeIfNeeded(size_t addBytes);

    size_t usedBytes_;
    size_t bufferSize_;
    std::unique_ptr<FixedBuffer> heapBuffer_;
    char stackBuffer_[N];
};

class Logger;

enum LogLevel { kTrace, kDebug, kInfo, kWarn, kError, kFatal };

// A line log info, usage is same as 'std::cout'.
// Log format in memory.
//  +------+-------+-----------+------+------+----------+------+
//  | time | level | thread id | logs | file | function | line |
//  +------+-------+-----------+------+------+----------+------+
class LogLine {
  public:
    LogLine(Logger *logger, LogLevel level, const char *file,
            const char *function, uint32_t line);
    ~LogLine();

    // operator various argument of logs type.
    LogLine &operator<<(bool arg);
    LogLine &operator<<(char arg);
    LogLine &operator<<(int32_t arg);
    LogLine &operator<<(uint32_t arg);
    LogLine &operator<<(int64_t arg);
    LogLine &operator<<(uint64_t arg);
    LogLine &operator<<(double arg);
    LogLine &operator<<(const char *arg);
    LogLine &operator<<(const std::string &arg);

    // stringify log level.
    const char *stringifyLevel(LogLevel level);

  private:
    Logger *logger_;
    LogLevel level_;
    const char *file_;
    const char *function_;
    uint32_t line_;
    SmartBuffer<256> buffer_;
};

// Write logs into file or stdout.
class LogFile {
  public:
    enum State { kStart, kEnd };

    // Logs to stdout.
    LogFile();

    // Logs to file.
    LogFile(const char *file, uint32_t rollSize);

    ~LogFile();

    // append bytes to FILE*
    void append(const char *str, size_t len) { doAppend_(str, len); }

    // append "len" bytes to a buffer.
    void appendToBuffer(const char *str, size_t len);

    // append "len" bytes to a stdout.
    void appendToStdout(const char *str, size_t len);

    // flush buffer to FILE*
    void flush() { doFlush_(); }

    // presist to file or stdout.
    void presist();

    static const size_t kBufferSize;
    static const uint32_t kMBytes;
    static const uint32_t kRollSize;
    static const uint32_t kInitBufferNumber;

  private:
    void rollFile();

    // write "len" bytes to file.
    ssize_t write(const char *str, size_t len);

    // consume producer buffer.
    void consume();

    void doPresist();

    FILE *fp_;
    uint32_t rollNumber_;
    const uint32_t rollBytes_;
    uint32_t writtenBytes_;
    std::string basename_;
    FixedBuffer produceBuffer_;
    std::vector<FixedBuffer> consumeBuffers_;
    std::thread thread_;
    std::mutex mutex_;
    State state_; // FIXED ME, atomic.
    int epollfd_;
    int eventfd_;
    std::function<void(const char *, size_t)> doAppend_;
    std::function<void()> doFlush_;
};

class Logger {
  public:
    Logger();
    Logger(const char *file, uint32_t rollSize);

    void append(const char *str, size_t len);
    void append(const char *str) { append(str, strlen(str)); }

    void flush();

  private:
    LogFile logFile_;
    std::mutex mutex_;
};

struct LogConfig {
    LogConfig() : rollsize(0) {}

    std::string filename;
    size_t rollsize;
};

LogConfig readConfig();

Logger *logger();
LogLevel loglevel();

} // namespace hquin

#define LOG(level)                                                             \
    if (hquin::loglevel() <= level)                                            \
    LogLine(hquin::logger(), level, __FILE__, __FUNCTION__, __LINE__)

#define LOG_TRACE LOG(hquin::kTrace)
#define LOG_DEBUG LOG(hquin::kDebug)
#define LOG_INFO LOG(hquin::kInfo)
#define LOG_WARN LOG(hquin::kWarn)
#define LOG_ERROR LOG(hquin::kError)
#define LOG_FATAL LOG(hquin::kFatal)
// File:    Log.h
// Author:  definezxh@163.com
// Date:    2019/07/03 20:31:34
// Desc:
//   A simple logger. only support log to file.
//   File format default is file.x.log, x is roll file number.
//   Log format like this:
//    [2019/07/04 15:42:26.221773][ERROR][log_test.cpp:main 14]LOG_ERROR

#pragma once

#include <string>
#include <memory>
#include <fstream>

namespace hquin {

enum LogLevel { INFO, WARN, ERROR };

class LogLine {
  public:
    LogLine(LogLevel level, const char *file, const char *function,
            uint32_t line);
    ~LogLine();

    LogLine &operator<<(char arg);
    LogLine &operator<<(int32_t arg);
    LogLine &operator<<(uint32_t arg);
    LogLine &operator<<(int64_t arg);
    LogLine &operator<<(uint64_t arg);
    LogLine &operator<<(double arg);
    LogLine &operator<<(const char *arg);
    LogLine &operator<<(const std::string &arg);

    void stringify(std::ofstream &ofs);

    // append log to buffer.
    template <typename T> void append(T arg);
    // void append(std::string &arg);
    // void append(const char *arg, size_t len);

    struct Literal {
        explicit Literal(const char *s) : str(s) {}
        const char *literal() { return str; }
        const char *str;
    };

  private:
    // get first unused bytes address.
    char *buffer() {
        return heapBuffer_ ? &(heapBuffer_.get())[usedBytes_]
                           : &stackBuffer_[usedBytes_];
    }

    // buffer begin address.
    char *begin() { return heapBuffer_ ? heapBuffer_.get() : stackBuffer_; }

    // buffer is char[] type, we need resize by ourself.
    void resizeIfNeeded(size_t addBytes);

    size_t usedBytes_;
    size_t bufferSize_;
    std::unique_ptr<char[]> heapBuffer_;
    //  perfer using stakc memory, reserve 8 bytes.
    char stackBuffer_[1024 - sizeof(size_t) * 2 - sizeof(heapBuffer_) - 8];

    static const size_t kInitBufferSize;
};

// write log info to file.
class FileWriter {
  public:
    FileWriter();
    FileWriter(std::string fileName, uint32_t rollFileBytes);

    // write LogLine to ofstream.
    void write(LogLine &line);

    static FileWriter &uniqueWriter();

  private:
    void rollFile(); // internal use, log new file.

    uint32_t rollFileBytes_;
    uint32_t rollFileNum_;
    uint32_t writenBytes_;
    std::string filename_;
    std::unique_ptr<std::ofstream> ofs_;

    static const uint32_t kRollFileBytes;
    static const std::string kFileName; // default file name.
};

class Logger {
  public:
    Logger() {}
    Logger &operator==(LogLine &line);

  private:
    // unique file writer as the logger。
    static std::unique_ptr<FileWriter> writer_;
};

#define LOG(level) Logger() == LogLine(level, __FILE__, __FUNCTION__, __LINE__)
#define LOG_INFO LOG(INFO)
#define LOG_WARN LOG(WARN)
#define LOG_ERROR LOG(ERROR)

} // namespace hquin

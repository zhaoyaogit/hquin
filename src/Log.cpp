// File:    Logger.cpp
// Author:  definezxh@163.com
// Date:    2019/07/03 21:55:25
// Desc:
//   A simple logger. only support log to file.
//   File format default is log20190805_x.log, x is roll file number.

#include <Log.h>
#include <Timestamp.h>
#include <Util.h>

#include <j4on/j4on.h>

#include <fcntl.h>

#include <libgen.h> // basename()
#include <stdio.h>
#include <assert.h>
#include <math.h>

namespace hquin {

FixedBuffer::FixedBuffer(size_t size)
    : usedBytes_(0), bufferSize_(size),
      buffer_(std::make_unique<char[]>(bufferSize_)) {}

void FixedBuffer::reset() { usedBytes_ = 0; }

void FixedBuffer::bzero() {
    memset(begin(), 0, bufferSize_);
    reset();
}

// @c string
void FixedBuffer::append(const char *arg, size_t len) {
    std::copy(arg, arg + len, buffer());
    usedBytes_ += len;
}

template <size_t N>
SmartBuffer<N>::SmartBuffer() : usedBytes_(0), bufferSize_(N) {}

template <size_t N> void SmartBuffer<N>::resizeIfNeeded(size_t addBytes) {
    const size_t requireBytes = usedBytes_ + addBytes;
    if (requireBytes <= bufferSize_)
        return;

    bufferSize_ = std::max(requireBytes, bufferSize_ * 2);
    if (!heapBuffer_) { // used stack buffer before now.
        heapBuffer_ = std::make_unique<FixedBuffer>(bufferSize_);
        std::copy(stackBuffer_, stackBuffer_ + usedBytes_,
                  heapBuffer_->begin());
    } else {
        std::unique_ptr<FixedBuffer> newHeapBuffer =
            std::make_unique<FixedBuffer>(bufferSize_);

        std::copy(heapBuffer_->begin(), heapBuffer_->begin() + usedBytes_,
                  newHeapBuffer->begin());

        std::swap(newHeapBuffer, heapBuffer_);
    }
}

template <size_t N> void SmartBuffer<N>::append(const char *arg, size_t len) {
    resizeIfNeeded(len);
    std::copy(arg, arg + len, buffer());
    usedBytes_ += len;
}

// append log line necessary info into buffer.
LogLine::LogLine(Logger *logger, LogLevel level, const char *file,
                 const char *function, uint32_t line)
    : logger_(logger), level_(level), file_((file)), function_(function),
      line_(line) {
    *this << stringifyLevel(level_) << ' ' << Timestamp::now().formatTimestamp()
          << ' ' << gettid() << "  ";
}

LogLine::~LogLine() {
    char *file = const_cast<char *>(file_);
    *this << " - " << ::basename(file) << ':' << function_
          << "():" << std::to_string(line_) << '\n';

    logger_->append(buffer_.begin(), buffer_.size());

    if (level_ == kFatal) {
        logger_->flush();
        abort();
    }
}

LogLine &LogLine::operator<<(bool arg) {
    if (arg)
        buffer_.append("true", 4);
    else
        buffer_.append("false", 5);
    return *this;
}

LogLine &LogLine::operator<<(char arg) {
    buffer_.append(&arg, 1);
    return *this;
}

LogLine &LogLine::operator<<(int32_t arg) {
    buffer_.append(std::to_string(arg).c_str());
    return *this;
}

LogLine &LogLine::operator<<(uint32_t arg) {
    buffer_.append(std::to_string(arg).c_str());
    return *this;
}

LogLine &LogLine::operator<<(int64_t arg) {
    buffer_.append(std::to_string(arg).c_str());
    return *this;
}

LogLine &LogLine::operator<<(uint64_t arg) {
    buffer_.append(std::to_string(arg).c_str());
    return *this;
}

LogLine &LogLine::operator<<(double arg) {
    buffer_.append(std::to_string(arg).c_str());
    return *this;
}

LogLine &LogLine::operator<<(const char *arg) {
    buffer_.append(arg);
    return *this;
}

LogLine &LogLine::operator<<(const std::string &arg) {
    buffer_.append(arg.c_str(), arg.length());
    return *this;
}

// stringify log level.
const char *LogLine::stringifyLevel(LogLevel level) {
    switch (level) {
    case kTrace:
        return "TRACE";
    case kDebug:
        return "DEBUG";
    case kInfo:
        return "INFO";
    case kWarn:
        return "WARN";
    case kError:
        return "ERROR";
    case kFatal:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

const size_t LogFile::kBufferSize = 65536;
const uint32_t LogFile::kMBytes = 1014 * 1024;
const uint32_t LogFile::kRollSize = 10; // MB
const uint32_t LogFile::kInitBufferNumber = 5;

LogFile::LogFile(const char *file, uint32_t rollSize)
    : fp_(NULL), rollNumber_(0), rollBytes_(rollSize * kMBytes),
      writtenBytes_(0), basename_(file), produceBuffer_(kBufferSize),
      thread_(&LogFile::presist, this), state_(kStart), epollfd_(-1),
      eventfd_(-1) {
    std::string fullname = basename_ + Timestamp::now().date() + '_' +
                           std::to_string(rollNumber_) + ".log";
    fp_ = ::fopen(fullname.c_str(), "w+");
    int err = ferror(fp_);
    if (err) {
        fprintf(stderr, "Faild to open file: %s", strerror(err));
        abort();
    }

    epollfd_ = ::epoll_create(1);
    if (epollfd_ < 0) {
        fprintf(stderr, "Faild to create epoll: %s", strerror(errno));
        abort();
    }

    eventfd_ = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (eventfd_ < 0) {
        fprintf(stderr, "Faild to create epoll: %s", strerror(errno));
        abort();
    }

    struct epoll_event event;
    event.data.fd = eventfd_;
    event.events = EPOLLIN;
    epoll_ctl(epollfd_, EPOLL_CTL_ADD, eventfd_, &event);

    doAppend_ = [&](const char *str, size_t len) { appendToBuffer(str, len); };
    doFlush_ = [&]() {
        doPresist();
        consume();
        doPresist();
    };

    for (uint32_t i = 0; i < kInitBufferNumber; ++i) {
        FixedBuffer buffer(kBufferSize);
        consumeBuffers_.push_back(std::move(buffer));
    }
}

LogFile::LogFile()
    : fp_(stdout), rollNumber_(0), rollBytes_(0), writtenBytes_(0),
      produceBuffer_(0) {
    doAppend_ = [&](const char *str, size_t len) { appendToStdout(str, len); };
    doFlush_ = [&]() { fflush(fp_); };
}

LogFile::~LogFile() {
    state_ = kEnd;
    ::eventfd_write(eventfd_, 1);
    flush();
    thread_.join();
}

void LogFile::appendToBuffer(const char *str, size_t len) {
    std::lock_guard guard(mutex_);

    if (produceBuffer_.size() + len > produceBuffer_.capacity()) {
        // put producer buffer to a free consumer buffer.
        consume();
        ::eventfd_write(eventfd_, 1);
    }

    produceBuffer_.append(str, len);
    writtenBytes_ += len;
}

void LogFile::appendToStdout(const char *str, size_t len) { write(str, len); }

ssize_t LogFile::write(const char *str, size_t len) {
    size_t n = fwrite(str, 1, len, fp_);
    size_t remain = len - n;
    while (remain > 0) {
        size_t x = fwrite(str + n, 1, remain, fp_);
        if (x == 0) {
            int err = ferror(fp_);
            if (err)
                fprintf(stderr, "File write error: %s", strerror(err));
            break;
        }
        n += x;
        remain -= x;
    }
    return len - remain;
}

void LogFile::presist() {
    struct epoll_event event;
    while (state_ == kStart) {
        int n = ::epoll_wait(epollfd_, &event, 1, -1);
        if (n == 1) {
            ::eventfd_read(eventfd_, NULL);
            doPresist();
        }
    }
}

void LogFile::doPresist() {
    std::lock_guard guard(mutex_);
    if (writtenBytes_ > rollBytes_)
        rollFile();

    for (auto &consume : consumeBuffers_) {
        if (!consume.isEmpty()) {
            write(consume.begin(), consume.size());
            consume.reset();
        }
    }
    fflush(fp_);
}

// Select a free consumer buffer to consume producer buffer data.
// If free consumer buffer is 0, add a consumer buffer.
void LogFile::consume() {
    size_t size = consumeBuffers_.size();
    for (size_t i = 0; i < size; ++i) {
        if (consumeBuffers_[i].isEmpty()) {
            std::swap(produceBuffer_, consumeBuffers_[i]);
            return;
        }
    }

    FixedBuffer buffer(kBufferSize);
    consumeBuffers_.push_back(std::move(buffer));
    std::swap(produceBuffer_, consumeBuffers_.back());
}

// filename + date + number + ".log"
void LogFile::rollFile() {
    fclose(fp_);

    std::string fullname = basename_ + Timestamp::now().date() + '_' +
                           std::to_string(++rollNumber_) + ".log";
    fp_ = ::fopen(fullname.c_str(), "w+");

    int err = ferror(fp_);
    if (err) {
        fprintf(stderr, "Faild to open file: %s", strerror(err));
        abort();
    }
    writtenBytes_ = 0;
}

Logger::Logger() {}
Logger::Logger(const char *file, uint32_t rollSize)
    : logFile_(file, rollSize) {}

void Logger::append(const char *str, size_t len) { logFile_.append(str, len); }

void Logger::flush() { logFile_.flush(); }

LogConfig readConfig() {
    LogConfig config;

    j4on::J4onParser parser("/etc/hquin.conf");
    parser.parse();

    j4on::Value v = parser.getValue("file_name");
    if (v.type() == j4on::kString) {
        std::string file =
            std::any_cast<j4on::String>(v.getAnyValue()).getString();
        config.filename = file;
    }

    v = parser.getValue("roll_size");
    if (v.type() == j4on::kNumber) {
        double num = std::any_cast<j4on::Number>(v.getAnyValue()).getNumber();
        config.rollsize = static_cast<size_t>(floor(num));
    }

    return config;
}

LogConfig config = readConfig();

Logger getLogger() {
    if (config.filename.length() > 0 && config.rollsize > 0)
        return Logger(config.filename.c_str(), config.rollsize);

    if (config.filename.length() > 0)
        return Logger(config.filename.c_str(), LogFile::kRollSize);

    return Logger();
}

Logger log = getLogger();

Logger *logger() { return &log; }

LogLevel loglevel() { return kDebug; }

} // namespace hquin

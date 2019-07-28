// File:    Logger.cpp
// Author:  definezxh@163.com
// Date:    2019/07/03 21:55:25
// Desc:
//   A simple logger. only support log to file.
//   File format default is log.x.txt, x is roll file number.

#include <Log.h>
#include <Timestap.h>

#include <string.h> //strlen()
#include <atomic>
#include <thread>

namespace hquin {

// LogLine implementation

// default buffer size
const size_t LogLine::kInitBufferSize = 256;

const char *toStringLevel(LogLevel level) {
    switch (level) {
    case INFO:
        return "INFO";
    case WARN:
        return "WARN";
    case ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

// append log line necessary info into buffer.
LogLine::LogLine(LogLevel level, const char *file, const char *function,
                 uint32_t line)
    : usedBytes_(0), bufferSize_(kInitBufferSize) {
    append(Timestap::now().formatTimestap());
    append(std::this_thread::get_id());
    append(file);
    append(function);
    append(line);
    append(toStringLevel(level));
}

LogLine::~LogLine() {}

// append a item log info to buffer.
template <typename T> void LogLine::append(T arg) {
    *reinterpret_cast<T *>(buffer()) = arg;
    usedBytes_ += sizeof(arg);
}

// a nonfixed log info occupy 2 item with (type, arg)
template <typename T> void LogLine::append(T arg, uint8_t argType) {
    resizeIfNeeded(sizeof(arg) + sizeof(argType));
    append(argType);
    append(arg);
}

// @c string
void LogLine::append(const char *arg) {
    uint32_t len = strlen(arg) + 1; // end of '\0'
    append(len, 7);

    resizeIfNeeded(len);
    std::copy(arg, arg + len, buffer());
    *(buffer() - 1) = '\0'; // ofstream <<
    usedBytes_ += len;
}

// std::string append
void LogLine::append(const std::string &arg) { append(arg.c_str()); }

// fetch one item log info to stream from buffer
template <typename T>
char *LogLine::fetch(std::ofstream &ofs, char *start, T *null) {
    T arg = *reinterpret_cast<T *>(start);
    ofs << arg;
    return start + sizeof(T);
}

// fetch and into ofstream
char *LogLine::fetch(std::ofstream &ofs, char *start) {
    uint32_t len = *reinterpret_cast<uint32_t *>(start);
    start += sizeof(len);
    ofs << start;
    return start + len;
}

char *LogLine::fetch(char **start) {
    char *p = *start;
    p += sizeof(uint8_t);                            // type
    uint32_t len = *reinterpret_cast<uint32_t *>(p); // string length
    p += sizeof(uint32_t);
    *start = p + len;
    return p;
}

// format log line to ofstream.
void LogLine::stringify(std::ofstream &ofs) {
    char *b = begin();
    const char *const end = buffer();

    // log head.
    char *time = fetch(&b); // timestap
    std::thread::id threadId =
        *reinterpret_cast<std::thread::id *>(b); // thread
    b += sizeof(std::thread::id);
    char *file = fetch(&b);                           // filename
    char *function = fetch(&b);                       // function
    uint32_t line = *reinterpret_cast<uint32_t *>(b); // line
    b += sizeof(uint32_t);
    char *level = fetch(&b); // log level.

    // format fixed log info to stream.
    ofs << '[' << time << "][" << threadId << "][" << level << ']' << '['
        << file << ':' << function << "(): " << line << "] ";

    // fetch nonfixed log info to stream.
    stringify(ofs, b, end);

    ofs << std::endl;
}

void LogLine::stringify(std::ofstream &ofs, char *start,
                        const char *const end) {

    if (start == end)
        return;

    uint8_t argType = static_cast<uint8_t>(*start);
    start++; // skip arg type

    switch (argType) {
    case 1:
        return stringify(ofs, fetch(ofs, start, static_cast<char *>(nullptr)),
                         end);
    case 2:
        return stringify(
            ofs, fetch(ofs, start, static_cast<int32_t *>(nullptr)), end);
    case 3:
        return stringify(
            ofs, fetch(ofs, start, static_cast<uint32_t *>(nullptr)), end);
    case 4:
        return stringify(
            ofs, fetch(ofs, start, static_cast<int64_t *>(nullptr)), end);
    case 5:
        return stringify(
            ofs, fetch(ofs, start, static_cast<uint64_t *>(nullptr)), end);
    case 6:
        return stringify(ofs, fetch(ofs, start, static_cast<double *>(nullptr)),
                         end);
    case 7:
        return stringify(ofs, fetch(ofs, start), end);
    }
}

LogLine &LogLine::operator<<(char arg) {
    append(arg, 1);
    return *this;
}

LogLine &LogLine::operator<<(int32_t arg) {
    append(arg, 2);
    return *this;
}

LogLine &LogLine::operator<<(uint32_t arg) {
    append(arg, 3);
    return *this;
}

LogLine &LogLine::operator<<(int64_t arg) {
    append(arg, 4);
    return *this;
}

LogLine &LogLine::operator<<(uint64_t arg) {
    append(arg, 5);
    return *this;
}

LogLine &LogLine::operator<<(double arg) {
    append(arg, 6);
    return *this;
}

LogLine &LogLine::operator<<(const char *arg) {
    append(arg);
    return *this;
}

LogLine &LogLine::operator<<(const std::string &arg) {
    append(arg.c_str());
    return *this;
}

void LogLine::resizeIfNeeded(size_t addBytes) {
    const size_t requireBytes = usedBytes_ + addBytes;
    if (requireBytes <= bufferSize_)
        return;

    bufferSize_ = std::max(requireBytes, bufferSize_ * 2);
    if (!heapBuffer_) { // used stack buffer before now.
        heapBuffer_ = std::make_unique<char[]>(bufferSize_);
        std::copy(stackBuffer_, stackBuffer_ + usedBytes_, heapBuffer_.get());
    } else {
        std::unique_ptr<char[]> newHeapBuffer =
            std::make_unique<char[]>(bufferSize_);
        std::copy(heapBuffer_.get(), heapBuffer_.get() + usedBytes_,
                  newHeapBuffer.get());
        std::swap(newHeapBuffer, heapBuffer_);
    }
}

// FileWriter implementation
const uint32_t FileWriter::kRollFileBytes = 10 * 1024 * 1024; // 10 MB
const std::string FileWriter::kFileName = "log"; // default file name.

// file name form is xxx.txt, if non-argument the name is log.
FileWriter::FileWriter() : FileWriter(kFileName, kRollFileBytes) {}

FileWriter::FileWriter(std::string filename, uint32_t rollFileBytes)
    : rollFileBytes_(rollFileBytes), rollFileNum_(0), writenBytes_(0),
      filename_(filename),
      ofs_(std::make_unique<std::ofstream>(
          filename_ + ".log", std::ofstream::trunc | std::ofstream::out)) {}

// write LogLine to file. if the log file size is greater then rollFileBytes_,
// then start logging new file.
void FileWriter::write(LogLine &line) {
    auto pos = ofs_->tellp();
    line.stringify(*ofs_);
    writenBytes_ += ofs_->tellp() - pos;
    if (writenBytes_ > rollFileBytes_) {
        rollFile();
    }
}

// when a file is reaching the set size, reset the FileWriter initialization.
void FileWriter::rollFile() {
    if (ofs_) {
        ofs_->flush();
        ofs_->close();
    }

    std::string filename = filename_ + std::to_string(++rollFileNum_) + ".log";
    writenBytes_ = 0;
    ofs_.reset(new std::ofstream);
    ofs_->open(filename, std::ofstream::trunc | std::ofstream::out);
}

// static variable, logger use only one file write;
std::unique_ptr<Logger> logger = std::make_unique<Logger>();
std::atomic<Logger *> log(logger.get());

Logger ::Logger()
    : state_(kInit), thread_(&Logger::write, this), queue_(1024),
      writer_(std::make_unique<FileWriter>()) {
    state_ = kReady;
}

Logger::~Logger() {
    state_ = kShutdown;
    thread_.join();
}

void Logger::add(LogLine &&line) { queue_.push(std::move(line)); }

void Logger::write() {
    while (state_ == kReady) {
        if (!queue_.isEmpty()) {
            LogLine line(UNKNOWN, "null", "null", 0);
            queue_.pop(line);
            writer_->write(line);
        } else {
            // reduce CPU use.
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}

void Log::operator==(LogLine &line) {
    log.load(std::memory_order_acquire)->add(std::move(line));
}

} // namespace hquin

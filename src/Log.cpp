// File:    Logger.cpp
// Author:  definezxh@163.com
// Date:    2019/07/03 21:55:25
// Desc:
//   A simple logger. only support log to file.
//   File format default is log.x.txt, x is roll file number.

#include <Log.h>
#include <Timestap.h>

namespace hquin {

// LogLine implementation

// default buffer size
const size_t LogLine::kInitBufferSize = 1024;

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

// init a log line necessary info
LogLine::LogLine(LogLevel level, const char *file, const char *function,
                 uint32_t line)
    : usedBytes_(0), bufferSize_(kInitBufferSize) {
    append(Literal(file));
    append(Literal(function));
    append(line);
    append(toStringLevel(level));
}

LogLine::~LogLine() {}

// T is built-in type.
template <typename T> void LogLine::append(T arg) {
    resizeIfNeeded(sizeof(arg));
    *reinterpret_cast<T *>(buffer()) = arg;
    usedBytes_ += sizeof(arg);
}

// format log line to ofstream.
void LogLine::stringify(std::ofstream &ofs) {
    char *b = begin();
    Literal file = *reinterpret_cast<Literal *>(b);
    b += sizeof(Literal);
    Literal function = *reinterpret_cast<Literal *>(b);
    b += sizeof(Literal);
    uint32_t line = *reinterpret_cast<uint32_t *>(b);
    b += sizeof(uint32_t);
    Literal level = *reinterpret_cast<Literal *>(b);
    b += sizeof(Literal);
    Literal log = *reinterpret_cast<Literal *>(b);
    b += sizeof(Literal);
    ofs << '[' << Timestap::now().formatTimestap() << ']' << '['
        << level.literal() << ']' << '[' << file.literal() << ':'
        << function.literal() << ' ' << line << ']' << log.literal() << '\n';
}

LogLine &LogLine::operator<<(char arg) {
    append(arg);
    return *this;
}

LogLine &LogLine::operator<<(int32_t arg) {
    append(arg);
    return *this;
}

LogLine &LogLine::operator<<(uint32_t arg) {
    append(arg);
    return *this;
}

LogLine &LogLine::operator<<(int64_t arg) {
    append(arg);
    return *this;
}

LogLine &LogLine::operator<<(uint64_t arg) {
    append(arg);
    return *this;
}

LogLine &LogLine::operator<<(double arg) {
    append(arg);
    return *this;
}

LogLine &LogLine::operator<<(const char *arg) {
    append(Literal(arg));
    return *this;
}

LogLine &LogLine::operator<<(const std::string &arg) {
    append(arg);
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
std::unique_ptr<FileWriter> Logger::writer_ = std::make_unique<FileWriter>();

// write log info to file.
Logger &Logger::operator==(LogLine &line) {
    writer_->write(line);
    return *this;
}

} // namespace hquin

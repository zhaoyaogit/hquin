// File:    HTTPRequest.cpp
// Author:  definezxh@163.com
// Date:    2019/07/23 08:59:21
// Desc:
//   Parse HTTP request.

#include <http/HTTPRequest.h>

namespace hquin {
namespace http {

HTTPRequest::HTTPRequest(Timestap receiveTime)
    : receiveTime_(receiveTime), method_(kInvalid), version_(kUnknown) {}

HTTPRequest::~HTTPRequest() {}

bool HTTPRequest::setMethod(const char *begin, const char *end) {
    std::string method(begin, end);
    if (method == "GET")
        method_ = kGet;
    else if (method == "POST")
        method_ = kPost;
    else
        method_ = kInvalid;
    return method_ != kInvalid;
}

const char *HTTPRequest::stringifyMethod() const {
    switch (method_) {
    case kGet:
        return "GET";
    case kPost:
        return "POST";
    default:
        return "INVALID";
    }
}

bool HTTPRequest::setVersion(const char *begin, const char *end) {
    std::string version(begin, end);
    if (version == "HTTP/1.0")
        version_ = kHTTP10;
    else if (version == "HTTP/1.1")
        version_ = kHTTP11;
    else
        version_ = kUnknown;
    return version_ != kUnknown;
}

const char *HTTPRequest::stringifyVersion() const {
    switch (version_) {
    case kHTTP10:
        return "HTTP/1.0";
    case kHTTP11:
        return "HTTP/1.1";
    default:
        return "UNKNOWN";
    }
}

// HTTP header
//   method | path | version
//   <field, value>,
void HTTPRequest::handle(const std::string &request) {
    std::string::size_type begin;
    std::string::size_type end;

    // parse header line
    end = request.find(' '); // method
    setMethod(request.data(), request.data() + end);

    begin = end + 1;
    end = request.find(' ', begin); // path
    setPath(request.data() + begin, request.data() + end);

    begin = end + 1;
    end = request.find('\r', begin); // version
    setVersion(request.data() + begin, request.data() + end);

    // header context
    while (1) {
        begin = end + 2; // \r\n

        end = request.find(' ', begin);
        if (end == std::string::npos)
            return;
        // request[end] == ':'
        std::string field(request.data() + begin, request.data() + end - 1);

        begin = end + 1; // white space
        end = request.find('\r', begin);
        if (end == std::string::npos)
            return;
        std::string value(request.data() + begin, request.data() + end);

        headers_.emplace(field, value);
    }
}

} // namespace http
} // namespace hquin

// File:    HTTPRequest.h
// Author:  definezxh@163.com
// Date:    2019/07/23 08:43:07
// Desc:
//   Parse HTTP request.

#pragma once

#include <Timestap.h>

#include <string>
#include <map>

namespace hquin {
namespace http {

// HTTP header
//   method | path | version
//   <field, value>,
class HTTPRequest {
  public:
    enum Method { kInvalid, kGet, kPost };
    enum Version { kUnknown, kHTTP10, kHTTP11 };

    HTTPRequest(Timestap receiveTime);
    ~HTTPRequest();

    Method method() const { return method_; }
    const char *stringifyMethod() const;
    bool setMethod(const char *begin, const char *end);

    Version version() const { return version_; }
    const char *stringifyVersion() const;
    bool setVersion(const char *begin, const char *end);

    const std::string &path() const { return path_; }
    void setPath(const char *begin, const char *end) {
        path_.assign(begin, end);
    }

    void setTimestap(Timestap timestap) { receiveTime_ = timestap; }
    Timestap receiveTime() const { return receiveTime_; }

    void handle(const std::string &request);

    std::string getHeader(const std::string &field) const {
        std::string result;
        std::map<std::string, std::string>::const_iterator it =
            headers_.find(field);
        if (it != headers_.end()) {
            result = it->second;
        }
        return result;
    }

    std::map<std::string, std::string> headers() const { return headers_; }

  private:
    Timestap receiveTime_;
    Method method_;
    Version version_;
    std::string path_;
    std::map<std::string, std::string> headers_;
};

} // namespace http
} // namespace hquin

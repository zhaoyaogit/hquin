// File:    HTTPResponse.h
// Author:  definezxh@163.com
// Date:    2019/07/23 16:28:51
// Desc:
//   Response http request.

#pragma once

#include <Buffer.h>

#include <string>
#include <map>

namespace hquin {
namespace http {

class HTTPResponse {
  public:
    enum StatusCode {
        kUnknown,
        kOK = 200,
        kBadRequest = 400,
        kForbidden = 403,
        kNotFound = 404,
        kNotImplemented = 501
    };

    explicit HTTPResponse(bool close)
        : status_(kUnknown), closeConnection_(close) {}

    StatusCode statusCode() const { return status_; }
    void setStatusCode(StatusCode status) { status_ = status; }

    void setStatusMessage(const std::string &message) { message_ = message; }
    const std::string &message() const { return message_; }

    void setBody(const std::string &body) { body_ = body; }
    const std::string body() const { return body_; }

    void setContentType(const std::string &contentType) {
        addHeader("Content-Type", contentType);
    }

    void addHeader(const std::string &field, const std::string &value) {
        headers_[field] = value;
    }

    void setCloseConnection(bool on) { closeConnection_ = on; }
    bool closeConnection() const { return closeConnection_; }

    void appendToBuffer(Buffer *buf) const;

  private:
    // http version, default HTTP/1.1
    StatusCode status_;
    bool closeConnection_;
    std::string message_;
    std::string body_; // response body
    std::map<std::string, std::string> headers_;
};

} // namespace http
} // namespace hquin

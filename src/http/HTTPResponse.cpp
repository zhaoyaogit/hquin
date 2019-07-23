// File:    HTTPResponse.h
// Author:  definezxh@163.com
// Date:    2019/07/23 16:28:51
// Desc:
//   Response http request.

#include <http/HTTPResponse.h>

namespace hquin {
namespace http {

void HTTPResponse::appendToBuffer(Buffer *buf) const {
    buf->append("HTTP/1.1");              // http version
    buf->append(std::to_string(status_)); // status code
    buf->append(message_);                // message
    buf->append("\r\n");

    if (closeConnection_) {
        buf->append("Connection: close\r\n");
    } else {
        buf->append("Content-Length:" + std::to_string(body_.size()) + "\r\n");
        buf->append("Connection: Keep-Alive\r\n");
    }

    for (const auto &header : headers_) {
        buf->append(header.first);
        buf->append(": ");
        buf->append(header.second);
        buf->append("\r\n");
    }

    buf->append("\r\n");
    buf->append(body_);
}

} // namespace http
} // namespace hquin
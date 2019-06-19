// File:    InetAddress.cpp
// Author:  definezxh@163.com
// Date:    2019/06/19 12:29:53
// Desc:
//   IPv4 internet socket address structure.

#include <InetAddress.h>

#include <arpa/inet.h>

#include <string.h>

namespace hquin {

InetAddress::InetAddress(uint16_t port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
}

InetAddress::InetAddress(const struct sockaddr_in &addr) : addr_(addr) {}

std::string InetAddress::stringifyHost() {
    char buf[32];
    char host[INET_ADDRSTRLEN] = "INVALID";
    uint16_t port = ntohs(addr_.sin_port);
    ::inet_ntop(AF_INET, &addr_.sin_addr.s_addr, host, sizeof(host));
    snprintf(buf, sizeof(buf), "%s:%u", host, port);
    return buf;
}

} // namespace hquin

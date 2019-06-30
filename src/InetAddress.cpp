// File:    InetAddress.cpp
// Author:  definezxh@163.com
// Date:    2019/06/19 12:29:53
// Desc:
//   IPv4 internet socket address structure.
//   Concentrate operations related to some socket function of parameter is
//   'struct sockaddr_in'.

#include <InetAddress.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <string.h>

namespace hquin {

InetAddress::InetAddress(uint16_t port, std::string address) {
    uint32_t addr;
    ::inet_pton(AF_INET, address.c_str(), &addr);
    memset(&inetAddr_, 0, sizeof(inetAddr_));
    inetAddr_.sin_family = AF_INET;
    inetAddr_.sin_port = htons(port);
    inetAddr_.sin_addr.s_addr = htonl(addr);
}

InetAddress::InetAddress(uint16_t port) {
    memset(&inetAddr_, 0, sizeof(inetAddr_));
    inetAddr_.sin_family = AF_INET;
    inetAddr_.sin_port = htons(port);
    inetAddr_.sin_addr.s_addr = htonl(INADDR_ANY);
}

InetAddress::InetAddress(const struct sockaddr_in &addr) : inetAddr_(addr) {}

std::string InetAddress::stringifyHost() const {
    char buf[32];
    char host[INET_ADDRSTRLEN] = "INVALID"; // set initial value
    ::inet_ntop(AF_INET, &inetAddr_.sin_addr, host, sizeof(host));
    snprintf(buf, sizeof(buf), "%s:%u", host, ntohs(inetAddr_.sin_port));
    return buf;
}

} // namespace hquin

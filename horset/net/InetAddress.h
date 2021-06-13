#pragma once

#include <netinet/in.h>

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);

    sa_family_t family() const { return addr_.sin_family; }

    void setSockAddr(struct sockaddr_in sockaddrIn) { addr_ = sockaddrIn; }

private:
    struct sockaddr_in addr_;
};



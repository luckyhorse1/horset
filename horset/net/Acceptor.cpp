//
// Created by xiaoma on 6/11/21.
//

#include "Acceptor.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

namespace {
    int createSocket(struct sockaddr_in &addr) {
        int fd = ::socket(addr.sin_family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
        if (fd < 0) {
            std::cerr << "sockets::createNonblockingOrDie" << std::endl;
        }
    }

    template<typename To, typename From>
    inline To implicit_cast(const From &f) {
        return f;
    }
}

Acceptor::Acceptor(EventLoop *loop, struct sockaddr_in &addr) : loop_(loop), socketfd_(createSocket(addr)),
                                                                acceptChannel_(loop, socketfd_), listening_(false) {
    auto buf = static_cast<const struct sockaddr *>(implicit_cast<const void *>(&addr));
    int ret = ::bind(socketfd_, buf, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    if (ret < 0) {
        std::cerr << "sockets::bind" << std::endl;
    }
    acceptChannel_.setReadCallback([this] { handleRead(); });
}

void Acceptor::listen() {
    loop_->assertInLoopThread();
    listening_ = true;
    int ret = ::listen(socketfd_, SOMAXCONN);
    if (ret < 0) {
        std::cerr << "sockets::listenOrDie" << std::endl;
    }

    acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
    loop_->assertInLoopThread();
    struct sockaddr_in addr;
    auto addrlen = static_cast<socklen_t>(sizeof addr);
    int connfd = ::accept4(socketfd_, static_cast<struct sockaddr *>(implicit_cast<void *>(&addr)),
                           &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, addr);
        } else {
            ::close(connfd);
        }
    } else {
        std::cerr << "in Acceptor::handleRead" << std::endl;
    }
}
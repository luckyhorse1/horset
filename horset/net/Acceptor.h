#pragma once

#include "Channel.h"
#include "InetAddress.h"

class Acceptor {
public:
    typedef std::function<void(int sockfd, InetAddress& address)> NewConnectionCallback;

    Acceptor(EventLoop *loop, const InetAddress &addr);

    void setNewConnectionCallback(const NewConnectionCallback &cb) { newConnectionCallback_ = cb; }

    void listen();

private:
    void handleRead();

    EventLoop *loop_;
    const int socketfd_;
    Channel acceptChannel_;
    bool listening_;
    NewConnectionCallback newConnectionCallback_;
};
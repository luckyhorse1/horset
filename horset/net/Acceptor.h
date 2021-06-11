#pragma once

#include "Channel.h"

class Acceptor {
public:
    typedef std::function<void(int sockfd, struct sockaddr_in)> NewConnectionCallback;

    Acceptor(EventLoop *loop, struct sockaddr_in &addr);

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
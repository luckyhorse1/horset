#pragma once

#include "Callbacks.h"
#include "InetAddress.h"

class Acceptor;

class EventLoop;

class TcpServer {
public:
    TcpServer(EventLoop *loop, const InetAddress &listenAddr);

    void start();

    void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

private:
    EventLoop *loop_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
};

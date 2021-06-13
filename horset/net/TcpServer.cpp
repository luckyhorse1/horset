//
// Created by xiaoma on 6/13/21.
//

#include "TcpServer.h"
#include "Acceptor.h"
#include "TcpConnection.h"

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr) : loop_(loop),
                                                                       acceptor_(new Acceptor(loop, listenAddr)) {
    acceptor_->setNewConnectionCallback([this](int socketfd, InetAddress &address) {
        TcpConnectionPtr connection;
        connectionCallback_(connection);
    });
}

void TcpServer::start() {
    loop_->runInLoop([this] { acceptor_->listen(); });
}
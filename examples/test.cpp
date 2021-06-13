#include "horset/net/EventLoop.h"
#include "horset/net/Channel.h"
#include "horset/net/Acceptor.h"
#include "horset/net/TcpServer.h"
#include <thread>
#include <sys/timerfd.h>
#include <cstring> // bzero
#include <unistd.h> // close
#include <iostream>
#include <netinet/in.h>
#include <memory> // shared_ptr

EventLoop *gLoop;

void func() {
    EventLoop loop;
    gLoop = &loop;
}

void testLoopNotInIO() {
    std::thread t(func);
    t.join();
    gLoop->loop();
}

void testOneLoopPerThread() {
    EventLoop loop;
    EventLoop loop2;
}

void timeout() {
    std::cout << "timeout!" << std::endl;
    gLoop->quit();
}

void testReactor() {
    EventLoop eventLoop;
    gLoop = &eventLoop;

    int timeFd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&eventLoop, timeFd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec t;
    bzero(&t, sizeof(t));
    t.it_value.tv_sec = 5;
    ::timerfd_settime(timeFd, 0, &t, NULL);

    eventLoop.loop();
    ::close(timeFd);
}

void timerCallback() {
    EventLoop loop;
    std::cout << "timer call back" << std::endl;
    gLoop->quit();
}

void testTimerQueue() {
    EventLoop loop;
    gLoop = &loop;
    loop.runAfter(2.0, timerCallback);
    loop.loop();
}

void func2() {
    gLoop->runInLoop([] { std::cout << "run in loop" << std::endl; });
}

void testRunInLoop() {
    EventLoop loop;
    gLoop = &loop;
    std::thread t(func2);
    t.join();
    loop.loop();
}

void func3(int sockfd, InetAddress& address) {
    ::write(sockfd, "how are you!\n", 13);
}

void testAcceptor() {
    EventLoop loop;
    InetAddress addr(8892);
    Acceptor acceptor(&loop, addr);
    acceptor.setNewConnectionCallback(func3);
    acceptor.listen();
    loop.loop();
}

void testTcpServer() {
    EventLoop loop;
    InetAddress addr(8892);
    TcpServer tcpServer(&loop, addr);
    tcpServer.setConnectionCallback(
            [](const TcpConnectionPtr &connection) { std::cout << "connection created" << std::endl; });
    tcpServer.start();
    loop.loop();
}

int main() {
    testTcpServer();
}

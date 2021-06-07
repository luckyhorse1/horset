#include "horset/net/EventLoop.h"
#include <thread>

EventLoop* gLoop;

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

int main() {
    testLoopNotInIO();
}

#pragma once

class EventLoop {
public:
    EventLoop();
    void loop();

private:
    int tid;
};

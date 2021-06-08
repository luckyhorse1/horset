#pragma once
#include <vector>

class Poller;
class Channel;

class EventLoop {
public:
    EventLoop();

    void loop();

    void updateChannel(Channel *channel);

    void quit() { quit_ = true; }

private:
    typedef std::vector<Channel*> ChannelList;

    int tid;
    Poller *poller_;
    ChannelList activeChannels;
    bool quit_ = false;
};

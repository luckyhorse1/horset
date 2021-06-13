set(NET_SRCS
        EventLoop.cpp
        Poller.cpp
        Channel.cpp
        Timer.cpp
        TimerQueue.cpp
        Acceptor.cpp
        InetAddress.cpp
        TcpServer.cpp
        TcpConnection.cpp
        poller/PollPoller.cpp
)

add_library(horset_net ${NET_SRCS})
target_link_libraries(horset_net horset_base)
set(NET_SRCS
        EventLoop.cpp
)

add_library(horset_net ${NET_SRCS})
target_link_libraries(horset_net horset_base)
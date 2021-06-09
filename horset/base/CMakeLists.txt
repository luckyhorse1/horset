set(BASE_SRCS
        CurrentThread.cpp
        Timestamp.cpp
)

add_library(horset_base ${BASE_SRCS})
target_link_libraries(horset_base pthread rt)
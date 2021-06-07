cmake_minimum_required(VERSION 2.6)

project(horset C CXX)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(horset/base)
add_subdirectory(horset/net)
add_subdirectory(examples)

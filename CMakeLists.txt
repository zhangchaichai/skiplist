cmake_minimum_required(VERSION 3.21)
project(skiplist)

set(CMAKE_CXX_STANDARD 14)

add_executable(skiplist main.cpp Node.h SkipList.h)

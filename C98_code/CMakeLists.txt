cmake_minimum_required(VERSION 3.7)
project(C98_code)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES main.cpp)
add_executable(C98_code ${SOURCE_FILES})
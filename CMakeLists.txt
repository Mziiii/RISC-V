cmake_minimum_required(VERSION 3.17)
project(RISC_V)

set(CMAKE_CXX_STANDARD 14)

add_executable(RISC_V main.cpp MemoryBox.h MyCpu.h Operator.h Parser.h)
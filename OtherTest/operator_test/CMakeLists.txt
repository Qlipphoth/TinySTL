# CMake 最低版本号要求
cmake_minimum_required(VERSION 2.8)

# set(CMAKE_C_COMPILER D:/codes/C++/mingw64/bin/gcc.exe)
# set(CMAKE_CXX_COMPILER D:/codes/C++/mingw64/bin/g++.exe)

# 项目信息
project(test)


# set(CMAKE_CXX_STANDARD 17)

# 指定生成目标
add_executable(test test.cpp)
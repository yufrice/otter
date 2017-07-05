cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -O2")
find_package(LLVM REQUIRED)
find_package(Boost 1.6 COMPONENTS program_options REQUIRED)


INCLUDE_DIRECTORIES(./inc)
add_executable(otterc
	otterc.cpp)
target_link_libraries(otterc stdc++fs)


cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -O2")

find_package(LLVM REQUIRED CONFIG)
INCLUDE_DIRECTORIES(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

find_package(Boost 1.6 COMPONENTS program_options REQUIRED)


INCLUDE_DIRECTORIES(./inc)
add_executable(otterc
	otterc.cpp
	./inc/codegen/generator.cpp)
llvm_map_components_to_libnames(llvm_libs support core bitwriter)
target_link_libraries(otterc stdc++fs ${llvm_libs})


cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++1z")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(LLVM_CONFIG_PATH "/usr/bin/llvm-config")

find_package(LLVM 4.0 REQUIRED Utils ${EXTRA_LLVM_MODULES} CONFIG)
INCLUDE_DIRECTORIES(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

find_package(Boost 1.6 COMPONENTS program_options REQUIRED)


INCLUDE_DIRECTORIES(./inc)
add_executable(otterc
	./lib/codegen/generator.cpp
	./lib/codegen/context.cpp
	./lib/semantics/preCheck.cpp
	otterc.cpp)
llvm_map_components_to_libnames(llvm_libs bitwriter passes objcarcopts)
target_link_libraries(otterc stdc++fs ${llvm_libs})


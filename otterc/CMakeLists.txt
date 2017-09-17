cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++1z")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(LLVM_CONFIG_PATH "/usr/bin/llvm-config")
execute_process(COMMAND ${LLVM_CONFIG_PATH} --includedir
	OUTPUT_VARIABL LLVM_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)

find_package(LLVM 5.0 REQUIRED ${EXTRA_LLVM_MODULES} CONFIG)
INCLUDE_DIRECTORIES(./inc ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

find_package(Boost 1.6 COMPONENTS program_options REQUIRED)

llvm_map_components_to_libnames(llvm_libs
# amdgpu
# amdgpuasmparser
# amdgpuasmprinter
# amdgpucodegen
# amdgpudesc
# amdgpudisassembler
# amdgpuinfo
# amdgpuutils
analysis
# asmparser
# asmprinter
# bitreader
# bitwriter
# bpf
# bpfasmprinter
# bpfcodegen
# bpfdesc
# bpfdisassembler
# bpfinfo
codegen
core
# coroutines
# coverage
# debuginfocodeview
# debuginfodwarf
# debuginfomsf
# debuginfopdb
# demangle
# engine
# executionengine
# globalisel
instcombine
instrumentation
# interpreter
ipo
# irreader
# libdriver
# lineeditor
# linker
# lto
mc
# mcdisassembler
# mcjit
# mcparser
# mirparser
# native
# nativecodegen
# nvptx
# nvptxasmprinter
# nvptxcodegen
# nvptxdesc
# nvptxinfo
objcarcopts
# object
# objectyaml
# option
# orcjit
# passes
# profiledata
# runtimedyld
scalaropts
# selectiondag
support
# symbolize
# tablegen
# target
transformutils
vectorize
# x86
x86asmparser
# x86asmprinter
x86codegen
x86desc
# x86disassembler
x86info
# x86utils
)


add_executable(otterc
	./lib/context/context.cpp
	./lib/semantics/preCheck.cpp
	./lib/codegen/generator.cpp
	./lib/codegen/context.cpp
	./lib/driver/driver.cpp
	otterc.cpp)
target_link_libraries(otterc stdc++fs ${llvm_libs})


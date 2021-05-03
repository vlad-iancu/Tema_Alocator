cmake_minimum_required(VERSION 3.17)
project(Tema_Alocator)

set(CMAKE_CXX_STANDARD 14)

add_executable(Tema_Alocator main.cpp PoolAllocator.h MemoryBlock.h test.h test.cpp Point.h)
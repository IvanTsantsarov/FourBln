cmake_minimum_required(VERSION 3.19)

project(FourBln LANGUAGES CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(FourBln
    main.cpp
    file.cpp
    includes.cpp
    fizzbuzz.s
)

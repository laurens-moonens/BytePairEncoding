cmake_minimum_required(VERSION 3.14)

project(bpe)

set (CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(bpe src/main.cpp src/BPE.cpp src/Flags.cpp)

target_compile_options(bpe PRIVATE -Werror -Wall -Wextra -funsigned-char)

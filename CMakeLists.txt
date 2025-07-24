cmake_minimum_required(VERSION 3.14)

project(bpe)

add_executable(bpe ../src/main.cpp)

target_compile_options(bpe PRIVATE -Werror -Wall -Wextra -funsigned-char)

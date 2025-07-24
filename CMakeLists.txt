cmake_minimum_required(VERSION 3.14)

project(BytePairEncoding)

add_executable(BytePairEncoding ../src/main.cpp)

target_compile_options(BytePairEncoding PRIVATE -Werror -Wall -Wextra -funsigned-char)

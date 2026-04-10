cmake_minimum_required(VERSION 3.10)
project(jcd C)

set(CMAKE_C_STANDARD 11)

add_executable(jcd src/main.c)

install(TARGETS jcd DESTINATION bin)
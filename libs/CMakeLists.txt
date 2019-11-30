cmake_minimum_required(VERSION 3.0.0)

add_subdirectory(Unity)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb/)

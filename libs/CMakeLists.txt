cmake_minimum_required(VERSION 3.0.0)

add_subdirectory(Unity)

add_library(stb INTERFACE)
# INTERFACE is because header-only lib. SYSTEM is to suppress warnings from -Wall
target_include_directories(stb SYSTEM INTERFACE stb/)

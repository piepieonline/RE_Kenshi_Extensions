cmake_minimum_required(VERSION 3.15)
project(EscortLib LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(EscortLib STATIC "./Include/Escort.h" "./Src/Escort.cpp")

target_include_directories(EscortLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")
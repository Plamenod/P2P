cmake_minimum_required(VERSION 2.8)

project(P2PManager)

file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

file(GLOB COMMON_HEADERS "../../common/*.h")
file(GLOB COMMON_SOURCES "../../common/*.cpp")

include_directories(../../common/)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

add_library(P2PManager ${HEADERS} ${SOURCES} ${COMMON_SOURCES} ${COMMON_HEADERS})
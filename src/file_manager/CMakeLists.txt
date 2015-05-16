cmake_minimum_required(VERSION 2.8)

project(FileManager)

file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

file(GLOB COMMON_HEADERS "../common/*.h")
file(GLOB COMMON_SOURCES "../common/*.cpp")

include_directories(../common/)


add_executable(FileManager ${HEADERS} ${SOURCES} ${COMMON_SOURCES} ${COMMON_HEADERS})
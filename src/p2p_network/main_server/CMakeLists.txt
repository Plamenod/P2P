cmake_minimum_required(VERSION 2.8)

project(P2PMainServer)

file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

include_directories(../../common/)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

add_executable(P2PMainServer ${HEADERS} ${SOURCES})
target_link_libraries(P2PMainServer Common)

if(WIN32)
	target_link_libraries(P2PMainServer wsock32 ws2_32)
endif()
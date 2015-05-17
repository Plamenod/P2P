cmake_minimum_required(VERSION 2.8)

add_subdirectory(file_manager)
add_subdirectory(p2p_network)

project(P2P)

file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

add_executable(P2P ${HEADERS} ${SOURCES})
target_link_libraries(P2P FileManager P2PManager)

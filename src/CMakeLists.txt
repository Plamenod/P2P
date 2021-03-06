cmake_minimum_required(VERSION 2.8)

add_subdirectory(file_manager)
add_subdirectory(p2p_network)
add_subdirectory(common)
add_subdirectory(tests)

project(P2P)

file(GLOB HEADERS "*.h" "app/*.h")
file(GLOB SOURCES "*.cpp" "app/*.cpp")

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -pthread ${CMAKE_CXX_FLAGS}")

include_directories(common)

add_executable(P2P ${HEADERS} ${SOURCES})
target_link_libraries(P2P FileManager P2PManager Common)

SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lpthread" )

if(WIN32)
	target_link_libraries(P2P wsock32 ws2_32)
endif()

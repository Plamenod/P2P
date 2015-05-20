project(Common)

file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

add_library(Common ${HEADERS} ${SOURCES})

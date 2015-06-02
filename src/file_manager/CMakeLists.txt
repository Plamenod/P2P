project(FileManager)

file(GLOB HEADERS "server/*.h" "client/*.h" "*.h")
file(GLOB SOURCES "server/*.cpp" "client/*.cpp" "*.cpp")

include_directories(../common/)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

add_library(FileManager ${HEADERS} ${SOURCES})
target_link_libraries(FileManager Common)
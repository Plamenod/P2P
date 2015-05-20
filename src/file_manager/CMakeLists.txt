project(FileManager)

file(GLOB HEADERS "server/*.h" "client/*.h" "*.h")
file(GLOB SOURCES "server/*.cpp" "client/*.cpp")

file(GLOB COMMON_HEADERS "../common/*.h")

include_directories(../common/)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

add_library(FileManager ${HEADERS} ${SOURCES} ${COMMON_SOURCES} ${COMMON_HEADERS})
target_link_libraries(FileManager Common)
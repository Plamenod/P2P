project(tests)

file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")

# compile the App class with the test for now
file(GLOB APP_HEADERS "../app/*.h")
file(GLOB APP_SOURCES "../app/*.cpp")

# compile main server with the test for now
file(GLOB MS_HEADERS "../p2p_network/main_server/p2p_main_server.h")
file(GLOB MS_SOURCES "../p2p_network/main_server/p2p_main_server.cpp")

if (WIN32)
    SOURCE_GROUP("Source Files\\MainServer" FILES "../p2p_network/main_server/p2p_main_server.cpp")
    SOURCE_GROUP("Header Files\\MainServer" FILES "../p2p_network/main_server/p2p_main_server.h")
    
    SOURCE_GROUP("Source Files\\App" FILES ${APP_SOURCES})
    SOURCE_GROUP("Header Files\\App" FILES ${APP_HEADERS})
endif()

include_directories(../common/)
include_directories(../app)
include_directories(../p2p_network/main_server)
include_directories(../p2p_network/p2p_manager)
include_directories(../file_manager)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -pthread ${CMAKE_CXX_FLAGS}")

add_executable(${PROJECT_NAME}
	${MS_HEADERS} ${MS_SOURCES}
	${APP_HEADERS} ${APP_SOURCES}
	${HEADERS} ${SOURCES}
)
target_link_libraries(${PROJECT_NAME} FileManager P2PManager Common)

SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lpthread")

if(WIN32)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()
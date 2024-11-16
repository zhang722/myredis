cmake_minimum_required(VERSION 3.5)

project(myredis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")

include_directories(include
                    /home/zhang/workspace/cpp/net/mmuduo/include
                    tests
)

aux_source_directory(./src SRC)

add_executable(redis_server src/RedisServer.cpp)

find_library(MMUDUO_LIB mmuduo PATHS /home/zhang/workspace/cpp/net/mmuduo/lib)
if (MMUDUO_LIB)
    message(STATUS "Found mmuduo library at ${MMUDUO_LIB}")
else()
    message(FATAL_ERROR "mmuduo library not found.")
endif()

target_link_libraries(redis_server ${MMUDUO_LIB} pthread)


aux_source_directory(./tests TEST_SRC)
add_executable(testAll ${TEST_SRC} ${SRC})
target_link_libraries(testAll ${MMUDUO_LIB})
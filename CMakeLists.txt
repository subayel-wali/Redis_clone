cmake_minimum_required(VERSION 3.12)
project(redis_clone LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(redis_server ${SOURCES})

if(WIN32)
    target_link_libraries(redis_server PRIVATE ws2_32)
else()
    find_package(Threads REQUIRED)
    target_link_libraries(redis_server PRIVATE Threads::Threads)
endif()

cmake_minimum_required(VERSION 3.10)
project(client)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(clnt ${SOURCES})
target_include_directories(clnt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(client main.cpp)

target_link_libraries(client PRIVATE clnt)
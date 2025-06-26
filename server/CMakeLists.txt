cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 17)

find_package(PostgreSQL REQUIRED)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(serv ${SOURCES})
target_include_directories(serv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(server main.cpp)

target_include_directories(server PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(server PRIVATE ${PostgreSQL_LIBRARIES} serv pqxx)

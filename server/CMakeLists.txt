cmake_minimum_required(VERSION 3.16)
project(server)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(serv STATIC ${SOURCES})
target_include_directories(serv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(serv PRIVATE Qt6::Core Qt6::Network Qt6::Sql)

qt_wrap_cpp(MOC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/include/server.hpp)
add_executable(server main.cpp ${MOC_SOURCES})
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(server PRIVATE serv Qt6::Core Qt6::Network Qt6::Sql)

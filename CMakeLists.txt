cmake_minimum_required (VERSION 3.10)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

project(Games)

aux_source_directory(. DIR_SRCS)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
include_directories(${PROJECT_SOURCE_DIR}/inc)

add_subdirectory(src)

add_executable(Games GameStart.c)

target_link_libraries(Games game)

message(STATUS ${PROJECT_SOURCE_DIR} "------COMPLETE!------")
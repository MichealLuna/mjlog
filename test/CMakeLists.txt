cmake_minimum_required(VERSION 3.10)

project(lgtest)

include_directories(/usr/include/mjlog)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)

add_executable(lgtest ${SRCS})

target_link_libraries(lgtest libmjlog.so)
cmake_minimum_required(VERSION 3.10)
project(mjlog)

#CMAKE_SOURCE_FILE变量会传递到子文件的CMakeLists中。
#子文件中使用CMAKE_CURRENT_SOURCE_FILE才对应子文件的CMakeLists。
ADD_SUBDIRECTORY(lib bin)
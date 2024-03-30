cmake_minimum_required(VERSION 3.10)

project(mjlog)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTRACE_")

#include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#header files
file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
#sorce files
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source SRCS)

message(STATUS "Header files: " ${HEADER_FILES})
message(STATUS "Source files:" ${SRCS})

add_library(mjlog SHARED ${SRCS})
set_target_properties(mjlog PROPERTIES OUTPUT_NAME "mjlog")
#cmake 在构建一个新target时，会尝试清理其它使用这个名字的库，如果没有清理还是只会构建动态库，没有静态库。
set_target_properties(mjlog PROPERTIES CLEAN_DIRECT_OUTPUT 1)

#VERSION是真正的版本，而SOVERSION是程序依赖的版本，cmake会创建符号链接指向真正的版本。
set_target_properties(mjlog PROPERTIES VERSION 1.0.0 SOVERSION 1)

# 关于动态库版本控制：在Linux下主要有可以有3个级别版本例如：libhello.so.1.0.2
#   形式为 libhello.so.x.y.z
#       x:主版本，接口、参数类型、参数个数都不变。
#       y:副版本，增加新的接口。
#       z:次次版本，内部实现变化，优化等。
#
# 在进行更新时，安装时，如果版本有变换，会进行符号链接。
#       libhello.so -> libhello.so.1
#       libhello.so.1 -> libhello.so.1.0.1
#       ibhello.so.1.0.1。
# 其实只有一个文件libhello.so.1.0.1。libhello.so , libhello.so.1都是符号链接。
#
#   在使用动态库写程序时，ld阶段不会找任何版本的动态文件只会找到libhello.so文件进行链接,(ld阶段
# 还有一件事，就是会查看libhello.so的soname字段，如果它为libhello.so.1，那么程序运行时，就会找
# libhello.so.1，而巧的是安装时会自动创建到一个libhello.so.1.0.1的链接，那么程序运行时找的库
# 其实是libhello.so.1.0.1。这不得不说，又是一层层的抽象增加灵活性。)
#

add_library(mjlog_static STATIC ${SRCS})
set_target_properties(mjlog_static PROPERTIES OUTPUT_NAME "mjlog")
set_target_properties(mjlog_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

INSTALL(FILES ${HEADER_FILES} DESTINATION include/mjlog)

#   LIBRARY指动态文件，ARCHIVE指静态文件。
INSTALL(TARGETS mjlog mjlog_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

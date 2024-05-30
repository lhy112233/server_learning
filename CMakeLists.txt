cmake_minimum_required(VERSION 3.2...5.0 FATAL_ERROR)

project(hy_server VERSION 0.1.0 DESCRIPTION "" HOMEPAGE_URL "" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
#MSVC use UTF-8
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:gbk>")
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/src)
# add_subdirectory(${CMAKE_SOURCE_DIR}/test)
# add_subdirectory(${CMAKE_SOURCE_DIR})
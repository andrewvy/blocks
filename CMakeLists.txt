cmake_minimum_required(VERSION 2.8)

project(blocks)

FILE(GLOB SOURCE_FILES src/*.c)

add_executable(
    blocks
    ${SOURCE_FILES}
    dependencies/glew/src/glew.c
    dependencies/lodepng/lodepng.c)

add_definitions(-std=c99 -O3)

add_subdirectory(dependencies/glfw)
include_directories(dependencies/lodepng)
include_directories(dependencies/glew/include)
include_directories(dependencies/glfw/include)

if(MINGW)
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}
        "C:/Program Files/CURL/lib" "C:/Program Files (x86)/CURL/lib")
    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH}
        "C:/Program Files/CURL/include" "C:/Program Files (x86)/CURL/include")
endif()

if(APPLE)
    target_link_libraries(blocks glfw
        ${GLFW_LIBRARIES})
endif()

if(UNIX)
    target_link_libraries(blocks dl glfw
        ${GLFW_LIBRARIES})
endif()

if(MINGW)
    target_link_libraries(blocks ws2_32.lib glfw
        ${GLFW_LIBRARIES})
endif()

cmake_minimum_required(VERSION 3.8)

set(This Message)

project(${This} CXX)

set(CMAKE_CXX_STANDARD 11)

set(Headers
    include/message/Http.hpp
)

set (Sources
    src/Http.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})
set_target_properties(${This} PROPERTIES
    FOLDER Libraries
)

target_include_directories(${This} PUBLIC include)

add_subdirectory(test)

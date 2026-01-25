cmake_minimum_required(VERSION 3.16)
project(text_editor)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(text_editor
        main.cpp
        src/GapBuffer.cpp
        libs/nativefiledialog/src/nfd_common.c
        libs/nativefiledialog/src/nfd_cocoa.m
)

target_include_directories(text_editor PRIVATE
        libs/nativefiledialog/src/include
)
find_library(COCOA_LIBRARY Cocoa)


target_link_libraries(text_editor
        PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        ${COCOA_LIBRARY}
)
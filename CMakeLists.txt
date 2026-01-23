cmake_minimum_required(VERSION 3.16)
project(text_editor)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(text_editor main.cpp)

target_link_libraries(text_editor
        PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)
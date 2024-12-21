cmake_minimum_required(VERSION 3.30)
project(DesktopWGL C)

set(CMAKE_C_STANDARD 17)

add_executable(DesktopWGL main.c)

target_link_libraries(DesktopWGL opengl32)
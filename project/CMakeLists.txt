cmake_minimum_required(VERSION 3.14)
project(project)

set(CMAKE_CXX_STANDARD 14)

add_executable(project glad.c main.cpp  Shader.cpp Shader.hpp)
set(SOURCE_FILES glad.c main.cpp)
target_link_libraries(project GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor assimp)
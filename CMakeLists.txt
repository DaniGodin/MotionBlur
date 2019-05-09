cmake_minimum_required(VERSION 3.14)
project(OPENGL)


set(CMAKE_CXX_STANDARD 14)

add_executable(OPENGL main.cpp matrix4.cpp matrix4.hh)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

target_link_libraries(OPENGL ${OPENGL_LIBRARIES} ${GLUT_LIBRARY}  ${GLEW_LIBRARIES})
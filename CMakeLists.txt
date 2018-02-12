cmake_minimum_required(VERSION 3.9)
project(OpenGL C)

set(CMAKE_C_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

add_executable(Test test/test.c)
target_link_libraries(Test ${OPENGL_LIBRARY} ${GLU_LIBRARY} ${GLUT_LIBRARY})

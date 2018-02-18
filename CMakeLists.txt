cmake_minimum_required(VERSION 3.9)
project(OpenGL C)

set(CMAKE_C_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

add_executable(Txt2dat src/txt2dat.c src/data.c)

add_executable(Test src/test.c)
target_link_libraries(Test ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(Opg01 src/opg01.c src/helpers.c src/data.c src/colors.c)
target_link_libraries(Opg01 m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(Opg02 src/opg02.c src/helpers.c src/data.c src/colors.c)
target_link_libraries(Opg02 m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

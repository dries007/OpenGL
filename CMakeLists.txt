cmake_minimum_required(VERSION 3.9)
project(OpenGL C)

set(CMAKE_C_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

add_executable(Txt2dat oef123/txt2dat.c oef123/data.c)

add_executable(Test oef123/test.c)
target_link_libraries(Test ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(Opg01 oef123/opg01.c oef123/helpers.c oef123/data.c oef123/colors.c)
target_link_libraries(Opg01 m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(Opg02 oef123/opg02.c oef123/helpers.c oef123/data.c oef123/colors.c)
target_link_libraries(Opg02 m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(Opg03 oef123/opg03.c oef123/helpers.c oef123/data.c oef123/colors.c)
target_link_libraries(Opg03 m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(Opg04 oef4/kubus.c)
target_link_libraries(Opg04 m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(Opg05 oef5/opg05.c oef5/helpers.c oef5/data.c oef5/colors.c)
target_link_libraries(Opg05 m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

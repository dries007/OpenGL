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

add_executable(Opg06 oef6/main.c oef6/colors.c oef6/helpers.c oef6/stl.c oef6/text.c)
target_link_libraries(Opg06 m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(template3D template3D/main.c template3D/colors.c template3D/colors.h template3D/helpers.c template3D/helpers.h template3D/text.c template3D/text.h template3D/stl.c template3D/stl.h)
target_link_libraries(template3D m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(project project/main.c project/colors.c project/colors.h project/helpers.c project/helpers.h project/text.c project/text.h project/user_input.c project/user_input.h)
target_link_libraries(project m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

add_executable(oef8 oef8/main.c oef8/colors.c oef8/colors.h oef8/helpers.c oef8/helpers.h oef8/text.c oef8/text.h oef8/stl.c oef8/stl.h)
target_link_libraries(oef8 m ${OPENGL_LIBRARY} ${GLUT_LIBRARY})

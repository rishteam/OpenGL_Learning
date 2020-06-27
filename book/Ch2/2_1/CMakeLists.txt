cmake_minimum_required(VERSION 3.6)

project(ch2_example)

add_executable(ch2
    main.cpp
)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

target_link_libraries(ch2
    ${OPENGL_LIBRARIES}
    GLUT::GLUT
    GLEW::GLEW
)
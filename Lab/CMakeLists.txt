cmake_minimum_required(VERSION 3.6)

project(Lab_example)

add_executable(Lab
    main.cpp
)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

target_link_libraries(Lab
    ${OPENGL_LIBRARIES}
    GLUT::GLUT
    GLEW::GLEW
)
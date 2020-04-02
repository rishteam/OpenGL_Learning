cmake_minimum_required(VERSION 3.6)

project(ch4_example)

add_executable(ch4
    4.2.Shader.cpp
)

# Add SFML
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

target_link_libraries(ch4
    ${OPENGL_LIBRARIES}
    sfml-system sfml-window sfml-graphics sfml-network sfml-audio
    GLUT::GLUT
    GLEW::GLEW
)
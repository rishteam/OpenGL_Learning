cmake_minimum_required(VERSION 3.10)

project(VictorTutorial)

include(cmake/macros.cmake)

add_subdirectory(deps/glad)
add_subdirectory(src)

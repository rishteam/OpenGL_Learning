cmake_minimum_required(VERSION 3.10)

project(glad)

add_library(glad STATIC src/glad.c)
add_library(glad::glad ALIAS glad)
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/include)

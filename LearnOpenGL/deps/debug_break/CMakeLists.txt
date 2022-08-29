cmake_minimum_required(VERSION 3.10)

project(debugbreak)

add_library(${PROJECT_NAME} STATIC debugbreak.cpp)
add_library(debugbreak::debugbreak ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC .)

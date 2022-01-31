cmake_minimum_required(VERSION 3.10)

project(ep_2_triangle)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} project_options project_libraries)
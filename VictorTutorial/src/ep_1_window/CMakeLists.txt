cmake_minimum_required(VERSION 3.10)

add_executable(ep_1 main.cpp)

target_include_directories(ep_1 PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(ep_1 project_options project_libraries)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(common)

add_library(${PROJECT_NAME}
    Buffer.cpp
    Shader.cpp
    Texture.cpp
    VertexArray.cpp
    Debug.cpp
)
add_library(common::common ALIAS common)

target_link_libraries(${PROJECT_NAME} glad::glad)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics)
find_package(fmt REQUIRED)
target_link_libraries(${PROJECT_NAME} fmt::fmt)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/)

cmake_minimum_required(VERSION 3.10)

project(ep_5_shaders)

add_executable(${PROJECT_NAME} main.cpp VertexArray.cpp VertexBuffer.cpp Utils.cpp Utils.h IndexBuffer.cpp IndexBuffer.h Ref.h Shader.cpp Shader.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} project_options project_libraries)
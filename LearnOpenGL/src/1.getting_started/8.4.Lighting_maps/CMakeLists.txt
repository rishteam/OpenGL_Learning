cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(8.4.Lighting_maps)
add_executable(${PROJECT_NAME} main.cpp shader.cpp buffer.cpp VertexArray.cpp camera.h player.h Texture2D.cpp)
target_link_libraries(${PROJECT_NAME} glad::glad)

# Link SFML
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
# Link OpenGL
find_package(OpenGL REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
# imgui-SFML
find_package(ImGui-SFML REQUIRED)
target_link_libraries(${PROJECT_NAME} ImGui-SFML::ImGui-SFML)
find_package(fmt REQUIRED)
target_link_libraries(${PROJECT_NAME} fmt::fmt)

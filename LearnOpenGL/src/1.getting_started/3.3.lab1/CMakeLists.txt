cmake_minimum_required(VERSION 3.10)

project(3.3.lab1)
add_executable(${PROJECT_NAME} main.cpp)
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

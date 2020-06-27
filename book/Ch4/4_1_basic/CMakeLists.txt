cmake_minimum_required(VERSION 3.6)

project(ch4_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug) # Debug Release

add_executable(ch4_1
    4.1.Basic.cpp
)

if(APPLE)
    set(SFML_STATIC_LIBRARIES False)
    set(SFML_DIR "/usr/local/Cellar/sfml/2.5.1")
    target_compile_definitions(SFMLTest PUBLIC "GL_SILENCE_DEPRECATION")
elseif(MSYS)
    set(SFML_STATIC_LIBRARIES True)
    set(SFML_DIR "/mingw32/lib/cmake/SFML")
else()
    message(FATAL_ERROR "Not supported")
endif()

# Add SFML
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(ch4_1
    sfml-system sfml-window sfml-graphics sfml-network sfml-audio
)
# Add ImGui-SFML
find_package(ImGui-SFML REQUIRED)
if(NOT ImGui-SFML_FOUND)
    message(FATAL_ERROR "ImGui-SFML not found")
endif()
target_link_libraries(ch4_1
    ImGui-SFML::ImGui-SFML
)
# Add OpenGL
find_package(OpenGL REQUIRED)
target_include_directories(ch4_1
    PUBLIC ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(ch4_1
    ${OPENGL_LIBRARIES}
)
# Add GLEW
find_package(GLEW REQUIRED)
target_link_libraries(ch4_1
    GLEW::GLEW
)

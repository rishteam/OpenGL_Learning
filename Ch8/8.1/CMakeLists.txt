cmake_minimum_required(VERSION 3.6)

project(ch8_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug) # Debug Release

add_executable(ch8
    src/main.cpp
    src/ViewManager.cpp
)

# link include file
target_include_directories(ch8 PRIVATE ${PROJECT_SOURCE_DIR}/include)

if(APPLE)
    set(SFML_STATIC_LIBRARIES False)
    set(SFML_DIR "/usr/local/Cellar/sfml/2.5.1")
    # set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
    # set(CMAKE_OSX_SYSROOT macosx10.12)
    target_compile_definitions(ch8 PUBLIC "GL_SILENCE_DEPRECATION")
elseif(MSYS)
    set(SFML_STATIC_LIBRARIES True)
    set(SFML_DIR "/mingw32/lib/cmake/SFML")
else()
    message(FATAL_ERROR "Not supported")
endif()

# Add SFML
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
target_link_libraries(ch8
    sfml-system sfml-window sfml-graphics sfml-network sfml-audio
)
# Add ImGui-SFML
find_package(ImGui-SFML REQUIRED)
if(NOT ImGui-SFML_FOUND)
    message(FATAL_ERROR "ImGui-SFML not found")
endif()
target_link_libraries(ch8
    ImGui-SFML::ImGui-SFML
)
# Add OpenGL
find_package(OpenGL REQUIRED)
target_include_directories(ch8
    PUBLIC ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(ch8
    ${OPENGL_LIBRARIES}
)
# Add GLEW
find_package(GLEW REQUIRED)
target_link_libraries(ch8
    GLEW::GLEW
)
# Add GLM
find_package(glm REQUIRED)
target_link_libraries(ch8
  glm
)


if(MSYS)
target_link_libraries(ch8
    # -static-libgcc -static-libstdc++ -lwsock32 -lws2_32
    # -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
    # -mconsole / -mwindows
)
endif()

# Copy shaders
file(GLOB SHADERS "shader/*.glsl")
file(COPY ${SHADERS} DESTINATION ${CMAKE_BINARY_DIR})

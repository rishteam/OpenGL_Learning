cmake_minimum_required(VERSION 3.10)

# Options
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17) # C++17

# Libraries
find_package(glfw3)

add_library(project_libraries INTERFACE)
target_link_libraries(project_libraries INTERFACE glad::glad glfw stb::stb)

list_subdir(SUBDIRS ${PROJECT_SOURCE_DIR}/src)
foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()

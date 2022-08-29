cmake_minimum_required(VERSION 3.10)

project(LearnOpenGL)

add_subdirectory(deps/glad)
add_subdirectory(deps/debug_break)

set(BUILD_SHARED_LIBS OFF CACHE BOOL force)
set(ASSIMP_BUILD_X3D_IMPORTER off)
set(ASSIMP_BUILD_X3D_EXPORTER off)
set(ASSIMP_BUILD_SMD_IMPORTER off)
set(ASSIMP_BUILD_GLTF_IMPORTER off)
set(ASSIMP_BUILD_GLTF_EXPORTER off)
add_subdirectory(deps/assimp)
unset(BUILD_SHARED_LIBS)

add_subdirectory(src/common)
add_subdirectory(src/1.getting_started)
add_subdirectory(src/2.lighting)
add_subdirectory(src/3.model_loading)

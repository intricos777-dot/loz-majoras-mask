cmake_minimum_required(VERSION 3.20)
project(loz-majoras-mask VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Self-contained copy of the Twilight Elysium engine subset used by this
# bridge (see third_party/te/NOTICE). No external build dependency.
set(TE_DIR ${CMAKE_SOURCE_DIR}/third_party/te)

add_library(te-engine STATIC
    ${TE_DIR}/src/engine/engine.cpp
    ${TE_DIR}/src/engine/memory.cpp
    ${TE_DIR}/src/engine/threading.h
    ${TE_DIR}/src/engine/input.cpp
    ${TE_DIR}/src/engine/resolution_scaler.cpp
    ${TE_DIR}/src/engine/math_types.cpp
    ${TE_DIR}/src/engine/entity.cpp
)
target_include_directories(te-engine PUBLIC ${TE_DIR}/src)

add_library(te-renderer STATIC
    ${TE_DIR}/src/renderer/renderer.cpp
    ${TE_DIR}/src/renderer/material/material.cpp
)
target_link_libraries(te-renderer PRIVATE te-engine)

add_library(te-shader STATIC
    ${TE_DIR}/src/shader/shader_pipeline.cpp
)
target_include_directories(te-shader PUBLIC ${TE_DIR}/assets/shaders)
target_link_libraries(te-shader PRIVATE te-engine)

add_library(te-pipeline STATIC
    ${TE_DIR}/src/pipeline/render_graph.cpp
)
target_include_directories(te-pipeline PUBLIC ${TE_DIR}/src/pipeline)
target_link_libraries(te-pipeline PRIVATE te-engine te-shader)

add_library(te-asset STATIC
    ${TE_DIR}/src/asset/asset_manager.cpp
)
target_link_libraries(te-asset PRIVATE te-engine)

add_executable(mm
    src/main.cpp
    src/engine_bridge.cpp
    src/time_cycle.cpp
    src/player_movement.cpp
    src/renderer_backend.cpp
)
target_link_libraries(mm PRIVATE te-engine te-renderer te-shader te-pipeline te-asset)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(mm PRIVATE -Wall -Wextra)
endif()

enable_testing()
add_test(NAME mm-core COMMAND mm --core)

# Headless module proof: GLB import, render graph cull, shader compile
add_executable(te-asset-smoke ${TE_DIR}/tests/asset_smoke_test.cpp)
target_compile_definitions(te-asset-smoke PRIVATE TE_SHADER_DIR="${TE_DIR}/assets/shaders")
target_link_libraries(te-asset-smoke PRIVATE te-engine te-asset te-pipeline te-shader)
add_test(NAME te-asset-smoke COMMAND te-asset-smoke)
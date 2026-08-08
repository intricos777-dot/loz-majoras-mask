cmake_minimum_required(VERSION 3.20)
project(loz-majoras-mask VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TE_DIR ${CMAKE_SOURCE_DIR}/../twilight-elysium)

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
    ${TE_DIR}/src/shader/shader_stub.cpp
)
target_link_libraries(te-shader PRIVATE te-engine)

add_library(te-pipeline STATIC
    ${TE_DIR}/src/pipeline/frame_graph_stub.cpp
)
target_link_libraries(te-pipeline PRIVATE te-engine)

add_library(te-asset STATIC
    ${TE_DIR}/src/asset/asset_stub.cpp
)
target_link_libraries(te-asset PRIVATE te-engine)

add_executable(mm
    src/main.cpp
    src/engine_bridge.cpp
)
target_link_libraries(mm PRIVATE te-engine te-renderer te-shader te-pipeline te-asset)

enable_testing()
add_test(NAME mm-core COMMAND mm)
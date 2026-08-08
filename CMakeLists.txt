cmake_minimum_required(VERSION 3.20)
project(loz-majoras-mask CXX)
set(CMAKE_CXX_STANDARD 20)
add_executable(loz-majoras-mask
  src/main.cpp
  src/engine_bridge.cpp
)
target_include_directories(loz-majoras-mask PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/../twilight-elysium/src
)

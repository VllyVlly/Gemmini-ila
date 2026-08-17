cmake_minimum_required(VERSION 3.15)

project(gemmini VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_subdirectory(ILAng)

add_executable(main src/main.cc src/gemmini.cc src/test_helpers.cc tests/test_config.cc tests/test_mv.cc tests/test_matmul.cc)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(main PRIVATE ilang::ilang)
cmake_minimum_required(VERSION 3.1)
project(clife)

add_library(clife_game STATIC src/clife_game.c)
target_include_directories(clife_game PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_compile_options(clife_game PRIVATE -Wall -Wextra -Wpedantic -O3)
target_compile_features(clife_game PRIVATE c_std_99)

include(CTest)
enable_testing()

add_subdirectory("tests")


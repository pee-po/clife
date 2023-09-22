cmake_minimum_required(VERSION 3.1.0)

link_libraries(clife_game)
include_directories(../include)
add_executable(allocation smoke/allocation.c)

add_test(NAME smoke_allocation COMMAND allocation)


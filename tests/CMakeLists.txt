cmake_minimum_required(VERSION 3.1.0)

link_libraries(clife_game)
include_directories(../include)
add_executable(allocation smoke/allocation.c)

include_directories(../src)
add_executable(serialise unit/serialise.c)

add_test(NAME smoke_allocation COMMAND allocation)
add_test(NAME unit_serialise_1 COMMAND serialise 004000 3 3 1 1)
add_test(NAME unit_serialise_2 COMMAND serialise 40A040 3 3 1 0 0 1 2 1 1 2)
add_test(
    NAME unit_serialise_3
    COMMAND serialise 8040000000008040 10 4 0 0 0 3 9 0 9 3
)


cmake_minimum_required(VERSION 3.1.0)

link_libraries(clife_game)
include_directories(../include)
add_executable(allocation smoke/allocation.c)

include_directories(../src)
include_directories(../tests/utils)
# Unit tests are to test elements of interface and their implementation
# i.e. one can check whether two methods of setting the board are consistent
# but one of those methods needs to be checked internaly, this we call unit
# test. Unit tests may need to be changed when internalls change.
add_executable(serialise unit/serialise.c)
# Interface tests are to check whether using elements of interface is
# consistent e.g. input == serialise(deserialise(input)). Interface tests
# do not need to be changed when internals change, only when interface changes.
add_executable(deserialise interface/deserialise.c)

add_test(NAME smoke_allocation_1 COMMAND allocation 3 3)
add_test(NAME smoke_allocation_2 COMMAND allocation 10 4)
add_test(NAME smoke_allocation_3 COMMAND allocation 24 31)
add_test(NAME unit_serialise_1 COMMAND serialise 004000 3 3 1 1)
add_test(NAME unit_serialise_2 COMMAND serialise 40A040 3 3 1 0 0 1 2 1 1 2)
add_test(
    NAME unit_serialise_3
    COMMAND serialise 8040000000008040 10 4 0 0 0 3 9 0 9 3
)
add_test(NAME deserialise_1 COMMAND deserialise 000000 3 3)
add_test(NAME deserialise_2 COMMAND deserialise 004000 3 3)
add_test(NAME deserialise_3 COMMAND deserialise 40A040 3 3)
add_test(NAME deserialise_4 COMMAND deserialise 8040000000008040 10 4)

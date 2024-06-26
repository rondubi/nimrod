cmake_minimum_required(VERSION 3.16)
project(nimrod VERSION 0.1.0)

add_library(nimrod_core
    "src/fuse.cc"
    "src/packet.cc"
    "src/queue.cc"
    "src/topology.cc"
    "src/receiver/round_robin.cc"
    "src/sender/black_hole.cc"
    "src/sender/fork.cc"
    "src/sender/log.cc"
    "src/sender/link.cc"
    "src/sender/time.cc"
    "src/rules/ipfw.cc"
)
target_compile_features(nimrod_core PRIVATE cxx_std_20)
target_include_directories(nimrod_core PUBLIC "src")

add_executable(nimrod
    "src/main.cc"
)
target_compile_features(nimrod PRIVATE cxx_std_20)
target_include_directories(nimrod PRIVATE "src")
target_link_libraries(nimrod PRIVATE nimrod_core)

add_executable(tests
    "tests/doctest_main.cc"
    "tests/queue.cc"
)
target_compile_features(tests PRIVATE cxx_std_17)
target_link_libraries(tests PRIVATE nimrod_core)
target_include_directories(tests PRIVATE "deps/doctest/doctest")
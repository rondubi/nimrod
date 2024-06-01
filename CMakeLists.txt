cmake_minimum_required(VERSION 3.16)
project(nimrod VERSION 0.1.0)

add_executable(nimrod
    "src/main.cc"
    "src/queue.cc"
    "src/receiver/round_robin.cc"
    "src/sender/fork.cc"
    "src/sender/log.cc"
    "src/sender/link.cc"
    "src/rules/ipfw.cc"
)
target_compile_features(nimrod PRIVATE cxx_std_20)
target_include_directories(nimrod PRIVATE "src")

add_executable(rules_tests
    "src/rules/ipfw_test.cc"
    "src/rules/ipfw.cc"
)
target_include_directories(rules_tests PRIVATE "src/rules" "src/")
target_compile_features(rules_tests PRIVATE cxx_std_20)
cmake_minimum_required(VERSION 3.16)
project(nimrod VERSION 0.1.0)

add_executable(nimrod
    "src/main.cc"
    "src/queue.cc"
    "src/receiver/round_robin.cc"
    "src/sender/fork.cc"
    "src/sender/log.cc"
    "src/sender/link.cc"
)
target_compile_features(nimrod PRIVATE cxx_std_20)
target_include_directories(nimrod PRIVATE "src")
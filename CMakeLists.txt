cmake_minimum_required(VERSION 3.16)
project(nimrod VERSION 0.1.0)

add_executable(nimrod
    "src/main.cc"
    "src/pipe.cc"
    "src/pipe2.cc"
    "src/fork.cc"
)
target_compile_features(nimrod PRIVATE cxx_std_20)
target_include_directories(nimrod PRIVATE "src")
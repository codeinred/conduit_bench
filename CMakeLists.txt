cmake_minimum_required(VERSION 3.0.0)
project(bench_test VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(run_benchmark main.cpp)
add_subdirectory(benchmark)
add_subdirectory(conduit)

include_directories(cppcoro/include)
target_link_libraries(run_benchmark conduit benchmark::benchmark)
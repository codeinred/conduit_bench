cmake_minimum_required(VERSION 3.0.0)
project(conduit_bench VERSION 0.1.0)

add_compile_options("-fPIC" "-std=c++20")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../lib")

set(POSITION_INDEPENDENT_CODE ON)

add_subdirectory(benchmark)
add_subdirectory(fmt)
add_subdirectory(tuplet)
include_directories(cppcoro/include)

add_executable(conduit_bench main.cpp)
target_link_libraries(conduit_bench tuplet fmt benchmark dl)

add_library(baseline SHARED MODULE "src/baseline.cpp")
target_link_libraries(baseline benchmark)

add_library(cppcoro SHARED MODULE "src/cppcoro.cpp")
target_link_libraries(cppcoro benchmark)
target_include_directories(cppcoro PRIVATE "conduit/include")

add_library(conduit-main SHARED MODULE "src/conduit.cpp")
target_include_directories(conduit-main PRIVATE "conduit/include")

add_library(conduit-dev SHARED MODULE "src/conduit.cpp")
target_include_directories(conduit-dev PRIVATE "conduit-dev/include")

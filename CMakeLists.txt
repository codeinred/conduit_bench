cmake_minimum_required(VERSION 3.0.0)
project(bench_test VERSION 0.1.0)

# find_package(benchmark REQUIRED)
add_subdirectory(benchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_compile_options("-stdlib=libc++" "-lc++abi")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  add_compile_options("-fcoroutines")
endif()

include_directories(conduit/include)
include_directories(cppcoro/include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(Threads REQUIRED)

include(CTest)
enable_testing()

add_executable(conduit_bench main.cpp)
target_link_libraries(conduit_bench benchmark::benchmark)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

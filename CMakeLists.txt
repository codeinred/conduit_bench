cmake_minimum_required(VERSION 3.0.0)
project(conduit_bench VERSION 0.1.0)

add_compile_options("-fPIC")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../lib")

set(POSITION_INDEPENDENT_CODE ON)

add_subdirectory(benchmark)
add_subdirectory(conduit)
add_subdirectory(fmt)
add_subdirectory(tuplet)
include_directories(cppcoro/include)

add_executable(conduit_bench main.cpp)
target_link_libraries(conduit_bench tuplet fmt benchmark conduit dl)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    file(GLOB all_examples "src/*.cpp")

    foreach(file_name ${all_examples})
        get_filename_component(test_name ${file_name} NAME_WE)
        add_library("${test_name}" SHARED MODULE "${file_name}")
        target_link_libraries("${test_name}" conduit benchmark)
    endforeach()
endif()



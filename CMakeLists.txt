cmake_minimum_required(VERSION 3.12.2)
project(sndfile_basic)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.66.0 REQUIRED COMPONENTS program_options)

find_library(SNDFILE
    libsndfile.a
    PATHS ENV PATH
    )
if(NOT SNDFILE)
    message(FATAL_ERROR "Could not find libsndfile")
endif()

set(all_sources
    src/main.cpp
    )

add_executable(sndfile_basic ${all_sources})

target_link_libraries(sndfile_basic Boost::program_options)

target_compile_options(sndfile_basic PUBLIC -Wall -Wextra -Werror -fcolor-diagnostics)

add_custom_target(tidy ALL
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND clang-tidy -quiet -p "${CMAKE_BINARY_DIR}" ${all_sources}
    COMMAND_EXPAND_LISTS
    )

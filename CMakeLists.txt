cmake_minimum_required(VERSION 3.12)
project(sndfile_basic)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.66.0 REQUIRED COMPONENTS program_options)

find_library(SNDFILE
    libsndfile.a
    PATHS ENV PATH
    )
if(NOT SNDFILE)
    message(FATAL_ERROR "Could not find libsndfile")
endif()

# add_library()

add_executable(sndfile_basic
    src/main.cpp
    src/args.cpp
)

target_link_libraries(sndfile_basic Boost::program_options)

target_compile_options(sndfile_basic PUBLIC -Wall -Wextra -Werror)

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

set(basic_sources
    src/basic/main.cpp
    )

set(sf2float_sources
    src/sf2float/main.cpp
    )

set(all_sources
    ${basic_sources}
    ${sf2float_sources}
    )

add_executable(basic ${basic_sources})
target_include_directories(basic PUBLIC src)
target_link_libraries(basic Boost::program_options)
target_compile_options(basic PUBLIC -Wall -Wextra -Werror -fcolor-diagnostics)

add_executable(sf2float ${sf2float_sources})
target_include_directories(sf2float PUBLIC src)
target_link_libraries(sf2float Boost::program_options)
target_compile_options(sf2float PUBLIC -Wall -Wextra -Werror -fcolor-diagnostics)

add_custom_target(tidy ALL
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND clang-tidy -quiet -p "${CMAKE_BINARY_DIR}" ${all_sources}
    COMMAND_EXPAND_LISTS
    )

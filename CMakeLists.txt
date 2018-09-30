cmake_minimum_required(VERSION 3.12.2)
project(sndfile_basic LANGUAGES CXX)
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

set(hello_sources
    src/hello/main.cpp
    )

set(basic_sources
    src/basic/main.cpp
    )

set(sf2float_sources
    src/sf2float/main.cpp
    )

set(randcopy_sources
    src/randcopy/main.cpp
    )

set(sfprop_sources
    src/sfprop/main.cpp
    )

set(sfgain_sources
    src/sfgain/main.cpp
    )

set(all_sources
    ${hello_sources}
    ${basic_sources}
    ${sf2float_sources}
    ${randcopy_sources}
    ${sfprop_sources}
    ${sfgain_sources}
    )

set(compiler_flags -Wall -Wextra -Werror)
if(CMAKE_CXX_COMPILER_ID EQUAL "Clang" OR CMAKE_CXX_COMPILER_ID EQUAL "AppleClang")
    string(APPEND compiler_flags -fcolor-diagnostics)
endif()

add_executable(hello ${hello_sources})
target_include_directories(hello PUBLIC src)
target_compile_options(hello PUBLIC ${compiler_flags})

add_executable(basic ${basic_sources})
target_include_directories(basic PUBLIC src)
target_link_libraries(basic Boost::program_options)
target_compile_options(basic PUBLIC ${compiler_flags})

add_executable(sf2float ${sf2float_sources})
target_include_directories(sf2float PUBLIC src)
target_link_libraries(sf2float Boost::program_options SNDFILE)
target_compile_options(sf2float PUBLIC ${compiler_flags})

add_executable(randcopy ${randcopy_sources})
target_include_directories(randcopy PUBLIC src)
target_link_libraries(randcopy Boost::program_options SNDFILE)
target_compile_options(randcopy PUBLIC ${compiler_flags})

add_executable(sfprop ${sfprop_sources})
target_include_directories(sfprop PUBLIC src)
target_link_libraries(sfprop Boost::program_options SNDFILE)
target_compile_options(sfprop PUBLIC ${compiler_flags})

add_executable(sfgain ${sfgain_sources})
target_include_directories(sfgain PUBLIC src)
target_link_libraries(sfgain Boost::program_options SNDFILE)
target_compile_options(sfgain PUBLIC ${compiler_flags})

add_custom_target(tidy
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND clang-tidy -quiet -p "${CMAKE_BINARY_DIR}" ${all_sources} -- ${CLANG_TIDY_EXTRA_FLAGS}
    COMMAND_EXPAND_LISTS
    )

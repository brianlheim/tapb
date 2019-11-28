cmake_minimum_required(VERSION 3.12.2)
project(sndfile_basic LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.66.0 REQUIRED COMPONENTS program_options)

find_library(vorbis libvorbis.a PATHS ENV PATH)
find_library(vorbisenc libvorbisenc.a PATHS ENV PATH)
find_library(vorbisfile libvorbisfile.a PATHS ENV PATH)
find_library(ogg libogg.a PATHS ENV PATH)
find_library(flac libflac.a PATHS ENV PATH)
find_library(sndfile libsndfile.a PATHS ENV PATH)

set(hello_sources src/hello/main.cpp)
set(basic_sources src/basic/main.cpp)
set(basic_bkpts_sources src/basic_bkpts/main.cpp)
set(sf2float_sources src/sf2float/main.cpp)
set(randcopy_sources src/randcopy/main.cpp)
set(sfprop_sources src/sfprop/main.cpp)
set(sfgain_sources src/sfgain/main.cpp)
set(sfnorm_sources src/sfnorm/main.cpp)
set(sfpan_sources src/sfpan/main.cpp)
set(envx_sources src/envx/main.cpp)
set(sfenv_sources src/sfenv/main.cpp)

set(all_sources
    ${hello_sources}
    ${basic_sources}
    ${basic_bkpts_sources}
    ${sf2float_sources}
    ${randcopy_sources}
    ${sfprop_sources}
    ${sfgain_sources}
    ${sfnorm_sources}
    ${sfpan_sources}
    ${envx_sources}
    ${sfenv_sources}
    )

set(compiler_flags -Wall -Wextra -Werror)
if(CMAKE_CXX_COMPILER_ID EQUAL "Clang" OR CMAKE_CXX_COMPILER_ID EQUAL "AppleClang")
    string(APPEND compiler_flags -fcolor-diagnostics)
endif()

set(audio_libs ${sndfile} ${vorbis} ${flac} ${ogg} ${vorbisenc} ${vorbisfile})

add_library(breakpoint STATIC src/breakpoint/breakpoint.cpp src/breakpoint/breakpoint.hpp)
target_include_directories(breakpoint PUBLIC src)
target_compile_options(breakpoint PUBLIC ${compiler_flags})

function(AddSimpleExe name sources)
    add_executable(${name} ${sources})
    target_include_directories(${name} PUBLIC src)
    target_compile_options(${name} PUBLIC ${compiler_flags})
    target_link_libraries(${name} PUBLIC Boost::program_options ${ARGN})
endfunction()

function(AddAudioExe name sources)
    AddSimpleExe(${name} ${sources})
    target_link_libraries(${name} PUBLIC ${audio_libs} ${ARGN})
endfunction()

AddSimpleExe(hello ${hello_sources})
AddSimpleExe(basic ${basic_sources})
AddSimpleExe(basic_bkpts ${basic_bkpts_sources} breakpoint)
AddAudioExe(sf2float ${sf2float_sources})
AddAudioExe(randcopy ${randcopy_sources})
AddAudioExe(sfprop ${sfprop_sources})
AddAudioExe(sfgain ${sfgain_sources})
AddAudioExe(sfnorm ${sfnorm_sources})
AddAudioExe(sfpan ${sfpan_sources} breakpoint)
AddAudioExe(envx ${envx_sources} breakpoint)
AddAudioExe(sfenv ${sfenv_sources} breakpoint)

include(CTest)
function(DefineTest name sources)
    add_executable(${name} ${sources})
    target_compile_options(${name} PUBLIC ${compiler_flags})
    target_include_directories(${name} PUBLIC src test)
    target_link_libraries(${name} PUBLIC breakpoint)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

DefineTest(test_breakpoint test/breakpoint/breakpoint.test.cpp)
DefineTest(test_stereo_envelope_generator test/util/stereo_envelope_generator.test.cpp)

# Currently broken
add_custom_target(tidy
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND clang-tidy -quiet -p "${CMAKE_BINARY_DIR}" ${all_sources} -- ${CLANG_TIDY_EXTRA_FLAGS}
    COMMAND_EXPAND_LISTS
    )

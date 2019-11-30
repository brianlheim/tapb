Solutions and elaborations on exercises in The Audio Programming Book (Boulanger, Lazzarini), in C++20
======================================================================================================

Author: Brian Heim

Created on: Tue Sep 11 20:34:17 CDT 2018

This is a repo with four goals:
- Do exercises from TAPB
- Get used to new C++17 and C++20 features
- Get experience using Catch2
- Use good engineering practices:
    - clang-tidy
    - clang-format
    - modern compiler and features
    - TDD
    - clean and modern CMake

Requirements
------------

- compiler with C++20 support
- CMake >= 3.12.2
- recent version of libsndfile (I am using 1.0.28)
- recent version of Boost::program\_options

Building
--------

Build like any CMake project. On macOS 10.14, I am using Clang 8 installed from Homebrew and the
toolchain-clang8.cmake toolchain file to build the project. So I do:

    mkdir build
    cd build
    cmake .. -G"Unix Makefiles" -DBoost_NO_BOOST_CMAKE=ON -DCMAKE_TOOLCHAIN_FILE=../toolchain-clang8.cmake \
        -DCMAKE_BUILD_TYPE=Debug
    cmake --build . --target all -- -j8

Adjust accordingly for your environment.

Tests use Catch2.

Executables
-----------

- `basic`: test for boost options wrapper.
- `basic_bkpts`: driver for breakpoints library.
- `envx`: turn a mono sound file into a breakpoint file
- `hello`: print out a hello message. toolchain tester.
- `sf2float`: copy a file to an output file (wav file)
- `sfgain`: copies an audio file, changing the gain
- `sfnorm`: normalizes an input file
- `sfpan`: pans an input mono file given a breakpoint file
- `sfprop`: print sound file properties
- `sfenv`: use a breakpoint file as an amplitude envelope for a given sound file

TODO
----

- use std::filesystem::path instead of std::string
- lift breakpoint validation to interface?

#pragma once

#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

#include <iostream>

template <typename F>
[[nodiscard]] static int checked_invoke_in_out( simple_options::options & opts,
                                                F && lambda ) noexcept {
    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "input" ) && opts.has( "output" ) ) {
        auto && input = opts["input"].as<std::string>();
        auto && output = opts["output"].as<std::string>();
        if ( lambda( input, output ) == SndfileErr::Success ) {
            return 0;
        } else {
            std::cout << "Operation failed." << std::endl;
            return 2;
        }
    } else {
        std::cout << opts;
        return 1;
    }
}

template <typename F>
[[nodiscard]] static int checked_invoke_in_out_bkpts( simple_options::options & opts,
                                                      F && lambda ) noexcept {
    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "input" ) && opts.has( "output" ) && opts.has( "breakpoints" ) ) {
        auto && input = opts["input"].as<std::string>();
        auto && output = opts["output"].as<std::string>();
        auto && breakpoints = opts["breakpoints"].as<std::string>();
        if ( lambda( input, output, breakpoints ) == SndfileErr::Success ) {
            return 0;
        } else {
            std::cout << "Operation failed." << std::endl;
            return 2;
        }
    } else {
        std::cout << opts;
        return 1;
    }
}

#pragma once

#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

#include <iostream>

template <typename F>
[[nodiscard]] static int checked_invoke( simple_options::options & opts, F && lambda ) {
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

#include <iostream>
#include <string>
#include <vector>

#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

#include "sndfile.hh"

constexpr size_t bufsize = 1024;

int main( int argc, char ** argv ) {
    Amplitude amp_scale;

    simple_options::options opts{ "sfgain", "Scale an audio file's amplitude" };
    opts.basic_option( "help,h", "Print description and exit" )
        .basic_option( "scale,a", "Amplitude scaling",
                       simple_options::defaulted_value( &amp_scale, 1.0 ) )
        .positional( "input", "Input file" )
        .positional( "output", "Output file" )
        .parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "input" ) && opts.has( "output" ) ) {
        auto && input = opts["input"].as<std::string>();
        auto && output = opts["output"].as<std::string>();
        if ( fwd_copy( scale_copy, input, output, amp_scale, bufsize ) == SndfileErr::Success ) {
            return 0;
        } else {
            std::cout << "Copy failed." << std::endl;
        }
    } else {
        std::cout << opts;
        return 1;
    }

    return 0;
}

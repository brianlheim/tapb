#include <iostream>
#include <string>
#include <vector>

#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

#include "sndfile.hh"

constexpr size_t bufsize = 1024;

SndfileErr do_copy_impl( SndfileHandle & from, SndfileHandle & to, const Amplitude amp_scale ) {
    std::vector<float> floats( from.channels() * bufsize );

    sf_count_t read = 0;
    sf_count_t total_written = 0;
    while ( ( read = from.readf( floats.data(), bufsize ) ) ) {
        std::transform( floats.begin(), floats.begin() + ( read * from.channels() ), floats.begin(),
                        [&amp_scale]( auto x ) { return x * amp_scale; } );
        auto written = to.writef( floats.data(), read );
        if ( written < read ) {
            std::cout << "Error while writing (" << written << " written): " << to.strError()
                      << std::endl;
            return SndfileErr::BadWrite;
        }

        total_written += written;
    }

    if ( total_written != from.frames() ) {
        std::cout << "Could not read entire file: " << from.strError() << std::endl;
        std::cout << "Read " << from.frames() << " | Wrote " << to.frames() << std::endl;
        return SndfileErr::BadRead;
    }

    return SndfileErr::Success;
}

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
        if ( fwd_copy( do_copy_impl, input, output, amp_scale ) == SndfileErr::Success ) {
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

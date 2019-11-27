#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "breakpoint/breakpoint.hpp"
#include "util/pan_utils.hpp"
#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

#include "sndfile.hh"

std::optional<std::vector<breakpoint::point>> get_breakpoints( SndfileHandle & from ) {
    const int bufsize = 1024;
    std::vector<float> floats( bufsize * 2 );

    sf_count_t read = 0;
    sf_count_t total_read = 0;
    // auto sample_rate = from.samplerate();

    while ( ( read = from.readf( floats.data(), bufsize ) ) ) {
        total_read += read;
    }

    if ( total_read != from.frames() ) {
        std::cout << "Could not read entire file: " << from.strError() << std::endl;
        std::cout << "Read " << total_read << " | But file has " << from.frames() << std::endl;
        return std::nullopt;
    }

    return std::nullopt;
}

SndfileErr extract_breakpoints( const std::string & from_path, const std::string & to_path ) {
    SndfileHandle from{ from_path, SFM_READ };
    if ( from.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open read file: " << from_path << std::endl;
        return SndfileErr::CouldNotOpen;
    }

    if ( from.channels() != 1 ) {
        std::cout << "Input file must be mono: " << from_path << std::endl;
        return SndfileErr::BadOperation;
    }

    auto && breakpoints = get_breakpoints( from );
    if ( !breakpoints ) {
        std::cout << "Error reading input file: " << from_path << std::endl;
        return SndfileErr::BadOperation;
    }

    if ( breakpoint::write_breakpoints( to_path, *breakpoints ) ) {
        return SndfileErr::Success;
    } else {
        std::cout << "Error writing breakpoints: " << to_path << std::endl;
        return SndfileErr::BadOperation;
    }
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "envx", "Extract a breakpoint file from a mono input file" };
    opts.basic_option( "help,h", "Print description and exit" )
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

        if ( extract_breakpoints( input, output ) == SndfileErr::Success ) {
            return 0;
        } else {
            std::cout << "Operation failed." << std::endl;
            return 1;
        }
    } else {
        std::cout << opts;
        return 1;
    }

    return 0;
}

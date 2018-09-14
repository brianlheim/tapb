#include <iostream>
#include <string>
#include <vector>

#include "util/simple_options.hpp"

#include "sndfile.hh"

bool do_copy( std::string from_path, std::string to_path ) {
    SndfileHandle from{ from_path, SFM_READ };
    if ( !from ) {
        std::cout << "Could not open read file: " << from_path << std::endl;
        return false;
    }

    SndfileHandle to{ to_path, SFM_WRITE, SF_FORMAT_FLOAT, from.channels(), from.samplerate() };
    if ( !to ) {
        std::cout << "Could not open write file: " << to_path << std::endl;
        return false;
    }

    std::vector<float> floats( from.channels() );

    while ( from.readf( floats.data(), 1 ) ) {
        auto written = to.writef( floats.data(), 1 );
        if ( written != 1 ) {
            std::cout << "Error while writing" << std::endl;
            return false;
        }
    }

    if ( to.frames() != from.frames() ) {
        std::cout << "Could not read entier file" << std::endl;
        return false;
    }

    return true;
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "my_program" };
    opts.basic_option( "help,h", "Print description and exit" )
        .positional( "input", "Input file" )
        .positional( "output", "Input file" )
        .parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "input" ) && opts.has( "output" ) ) {
        if ( do_copy( opts["input"].as<std::string>(), opts["output"].as<std::string>() ) ) {
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

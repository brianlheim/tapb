#include <iostream>
#include <string>
#include <vector>

#include "util/simple_options.hpp"

#include "sndfile.hh"

std::ostream & operator<<( std::ostream & os, const SndfileHandle handle ) {
    os << "Format:      " << handle.format() << std::endl;
    os << "Frames:      " << handle.frames() << std::endl;
    os << "Channels:    " << handle.channels() << std::endl;
    os << "Sample rate: " << handle.samplerate() << std::endl;
    return os;
}

bool do_copy_impl( SndfileHandle & from, SndfileHandle & to, const size_t bufsize ) {
    std::vector<float> floats( from.channels() * bufsize );

    sf_count_t read = 0;
    sf_count_t total_written = 0;
    while ( ( read = from.readf( floats.data(), bufsize ) ) ) {
        auto written = to.writef( floats.data(), read );
        if ( written < read ) {
            std::cout << "Error while writing (" << written << " written): " << from.strError()
                      << std::endl;
            return false;
        }

        total_written += written;
    }

    if ( total_written != from.frames() ) {
        std::cout << "Could not read entire file: " << to.strError() << std::endl;
        std::cout << "Read " << from.frames() << " | Wrote " << to.frames() << std::endl;
        return false;
    }

    return true;
}

bool do_copy( std::string from_path, std::string to_path, const size_t bufsize ) {
    SndfileHandle from{ from_path, SFM_READ };
    if ( !from ) {
        std::cout << "Could not open read file: " << from_path << std::endl;
        return false;
    }

    SndfileHandle to{ to_path, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, from.channels(),
                      from.samplerate() };
    if ( !to ) {
        std::cout << "Could not open write file: " << to_path << std::endl;
        return false;
    }

#ifndef NDEBUG
    std::cout << from << "\n" << to << "\n";
#endif

    return do_copy_impl( from, to, bufsize );
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "my_program" };
    size_t bufsize;
    opts.basic_option( "help,h", "Print description and exit" )
        .basic_option( "bufsize,b", "Buffer size in frames",
                       simple_options::value<size_t>( &bufsize )->default_value( 1 ) )
        .positional( "input", "Input file" )
        .positional( "output", "Input file" )
        .parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "input" ) && opts.has( "output" ) ) {
        if ( do_copy( opts["input"].as<std::string>(), opts["output"].as<std::string>(), bufsize ) ) {
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

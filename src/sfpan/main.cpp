#include <iostream>
#include <string>
#include <vector>

#include "breakpoint/breakpoint.hpp"
#include "util/pan_utils.hpp"
#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

#include "sndfile.hh"

SndfileErr pan_copy( SndfileHandle & from,
                     SndfileHandle & to,
                     const double position,
                     const size_t bufsize = 1024 ) {
    // allocate enough for stereo
    std::vector<float> floats( bufsize * 2 );

    sf_count_t read = 0;
    sf_count_t total_written = 0;
    auto pan = constant_power_pan<double>( position );
    while ( ( read = from.readf( floats.data(), bufsize ) ) ) {
        for ( auto i = read; i > 0; --i ) {
            floats[i * 2 - 1] = floats[i - 1] * pan.right;
            floats[i * 2 - 2] = floats[i - 1] * pan.left;
        }
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

SndfileErr fwd_pan_copy( const std::string & from_path,
                         const std::string & to_path,
                         double position ) {
    SndfileHandle from{ from_path, SFM_READ };
    if ( from.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open read file: " << from_path << std::endl;
        return SndfileErr::CouldNotOpen;
    }

    if ( from.channels() != 1 ) {
        std::cout << "Input file must be mono: " << from_path << std::endl;
        return SndfileErr::BadOperation;
    }

    SndfileHandle to{ to_path, SFM_WRITE, from.format(), 2, from.samplerate() };
    if ( to.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open write file: " << to_path << std::endl;
        return SndfileErr::CouldNotOpen;
    }

#ifndef NDEBUG
    std::cout << from << "\n" << to << "\n";
#endif

    return pan_copy( from, to, position );
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "sfpan", "Pan a mono file at a fixed stereo position" };
    double position;
    opts.basic_option( "help,h", "Print description and exit" )
        .basic_option( "pos,p", "Pan position, -1 <= p <= 1",
                       simple_options::defaulted_value( &position, 0.0 ) )
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
        if ( !( position >= -1.0 && position <= 1.0 ) ) {
            std::cout << "Pan position must be -1 <= p <= 1." << std::endl;
            return 1;
        }

        if ( fwd_pan_copy( input, output, position ) == SndfileErr::Success ) {
            return 0;
        } else {
            std::cout << "Copy failed." << std::endl;
            return 1;
        }
    } else {
        std::cout << opts;
        return 1;
    }

    return 0;
}

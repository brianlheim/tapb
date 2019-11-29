#include <iostream>
#include <string>
#include <vector>

#include "util/checked_invoke.hpp"
#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

bool do_copy_impl( SndfileHandle & from,
                   SndfileHandle & to,
                   const size_t bufsize,
                   std::vector<float> & floats ) {
    sf_count_t read = 0;
    sf_count_t total_written = 0;
    while ( ( read = from.readf( floats.data(), bufsize ) ) ) {
        auto written = to.writef( floats.data(), read );
        if ( written < read ) {
            std::cout << "Error while writing (" << written << " written): " << to.strError()
                      << std::endl;
            return false;
        }

        total_written += written;
    }

    if ( total_written != from.frames() ) {
        std::cout << "Could not read entire file: " << from.strError() << std::endl;
        std::cout << "Read " << from.frames() << " | Wrote " << to.frames() << std::endl;
        return false;
    }

    return true;
}

bool do_copy_repeated( SndfileHandle & from,
                       SndfileHandle & to,
                       const size_t bufsize,
                       const size_t repeats ) {
    std::vector<float> floats( from.channels() * bufsize );

    for ( auto i = 0ul; i < repeats; ++i ) {
        auto const result = do_copy_impl( from, to, bufsize, floats );
        if ( !result ) {
            return false;
        }

        from.seek( 0, 0 );
    }

    return true;
}

bool do_copy( const std::string & from_path,
              const std::string & to_path,
              const size_t bufsize,
              const size_t repeats ) {
    SndfileHandle from{ from_path, SFM_READ };
    if ( from.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open read file: " << from_path << std::endl;
        return false;
    }

    SndfileHandle to{ to_path, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, from.channels(),
                      from.samplerate() };
    if ( to.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open write file: " << to_path << std::endl;
        return false;
    }

#ifndef NDEBUG
    std::cout << from << "\n" << to << "\n";
#endif

    return do_copy_repeated( from, to, bufsize, repeats );
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "sf2float" };
    size_t bufsize, repeats;
    opts.basic_option( "help,h", "Print description and exit" )
        .basic_option( "bufsize,b", "Buffer size in frames",
                       simple_options::defaulted_value( &bufsize, 1 ) )
        .basic_option( "repeats,r", "Number of times to repeat",
                       simple_options::defaulted_value( &repeats, 1 ) )
        .positional( "input", "Input file" )
        .positional( "output", "Output file" )
        .parse( argc, argv );

    using namespace std::placeholders;
    return checked_invoke_in_out( opts, std::bind( do_copy, _1, _2, bufsize, repeats ) );
}

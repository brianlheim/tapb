#include <iostream>
#include <string>
#include <vector>

#include "util/checked_invoke.hpp"
#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

bool do_copy_repeated( SndfileHandle & from,
                       SndfileHandle & to,
                       const size_t bufsize,
                       const size_t repeats ) {
    for ( auto i = 0ul; i < repeats; ++i ) {
        auto result = transform_copy(
            from, to, []( auto ) {}, bufsize );
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
    auto from = make_input_handle( from_path );
    auto to = make_output_handle( to_path, from, SF_FORMAT_WAV | SF_FORMAT_FLOAT );
    return from && to ? do_copy_repeated( *from, *to, bufsize, repeats ) : false;
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

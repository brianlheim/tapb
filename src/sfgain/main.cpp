#include "util/checked_invoke.hpp"

#include <functional>

int main( int argc, char ** argv ) {
    Amplitude amp_scale;
    constexpr size_t bufsize = 1024;

    simple_options::options opts{ "sfgain", "Scale an audio file's amplitude" };
    opts.basic_option( "help,h", "Print description and exit" )
        .basic_option( "scale,a", "Amplitude scaling",
                       simple_options::defaulted_value( &amp_scale, 1.0 ) )
        .positional( "input", "Input file" )
        .positional( "output", "Output file" )
        .parse( argc, argv );

    using namespace std::placeholders;
    return checked_invoke_in_out( opts, [amp_scale, bufsize]( auto & input, auto & output ) {
        return fwd_copy( scale_copy, input, output, amp_scale, bufsize );
    } );
}

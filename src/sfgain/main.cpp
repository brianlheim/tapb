#include "util/checked_invoke.hpp"

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

    // TODO function names
    return checked_invoke_in_out(
        opts, [bufsize, amp_scale]( const std::string & input, const std::string & output ) {
            return fwd_copy( scale_copy, input, output, amp_scale, bufsize );
        } );
}

#include "util/checked_invoke.hpp"
#include "util/sndfile_utils.hpp"

#include <functional>

static bool fwd_scale_copy( const std::string & from_path,
                            const std::string & to_path,
                            const Amplitude amp_scale,
                            const size_t bufsize ) noexcept {
    auto from = make_input_handle( from_path );
    auto to = make_output_handle( to_path, from );
    return from && to ? scale_copy( *from, *to, amp_scale, bufsize ) : false;
}

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
    return checked_invoke_in_out( opts, std::bind( fwd_scale_copy, _1, _2, amp_scale, bufsize ) );
}

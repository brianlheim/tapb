#include <string>
#include <vector>

#include "breakpoint/breakpoint.hpp"
#include "util/basic_envelope_generator.hpp"
#include "util/checked_invoke.hpp"
#include "util/pan_utils.hpp"
#include "util/sndfile_utils.hpp"

static void multichan_multiply( std::span<float> & out,
                                const std::span<float> & in,
                                int num_channels ) {
    sf_count_t i = 0;
    for ( auto amp : in )
        for ( int chan = 0; chan < num_channels; i++, chan++ )
            out[i] *= amp;
}

static bool apply_breakpoints_impl( SndfileHandle & from,
                                    SndfileHandle & to,
                                    const breakpoint::point_list & points,
                                    const size_t bufsize = 1024 ) {
    basic_envelope_generator gen( points, from.samplerate(), bufsize );
    return transform_copy( from, to, [&gen, &from]( auto span ) {
        multichan_multiply( span, gen.next_frames( span.size() ), from.channels() );
    } );
}

static void normalize( breakpoint::point_list & points ) {
    auto max = max_point( points.begin(), points.end() );
    for ( auto x : points )
        x.value /= max.value;
}

static bool apply_breakpoints( const std::string & from_path,
                               const std::string & to_path,
                               const std::string & breakpoints_path,
                               bool do_normalize ) {
    auto from = make_input_handle( from_path );
    auto to = make_output_handle( to_path, from );
    if ( !from || !to ) {
        return false;
    }

    auto breakpoints = breakpoint::parse_breakpoints( breakpoints_path );
    if ( auto * perr = std::get_if<breakpoint::parse_error>( &breakpoints ) ) {
        std::cout << "Error parsing breakpoint file '" << breakpoints_path << "': " << *perr
                  << std::endl;
        return false;
    } else if ( auto * pvals = std::get_if<breakpoint::point_list>( &breakpoints ) ) {
        if ( do_normalize )
            normalize( *pvals );
        return apply_breakpoints_impl( *from, *to, *pvals );
    } else {
        std::cout << "Unknown error while parsing breakpoints" << std::endl;
        return false;
    }
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "sfenv",
                                  "Apply a breakpoint file as an envelope on an input file" };
    opts.basic_option( "help,h", "Print description and exit" )
        .basic_option( "normalize,n", "Normalize breakpoints first" )
        .positional( "input", "Input file" )
        .positional( "output", "Output file" )
        .positional( "breakpoints", "Breakpoint file" )
        .parse( argc, argv );

    using namespace std::placeholders;
    return checked_invoke_in_out_bkpts(
        opts, std::bind( apply_breakpoints, _1, _2, _3, opts.has( "normalize" ) ) );
}

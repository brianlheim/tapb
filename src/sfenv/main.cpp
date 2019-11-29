#include <string>
#include <vector>

#include "breakpoint/breakpoint.hpp"
#include "util/basic_envelope_generator.hpp"
#include "util/checked_invoke.hpp"
#include "util/pan_utils.hpp"

static void multichan_multiply( std::vector<float> & out,
                                const std::span<float> & in,
                                int num_channels ) {
    sf_count_t i = 0;
    for ( auto amp : in )
        for ( int chan = 0; chan < num_channels; i++, chan++ )
            out[i] *= amp;
}

static SndfileErr pan_copy( SndfileHandle & from,
                            SndfileHandle & to,
                            const std::vector<breakpoint::point> & points,
                            const size_t bufsize = 1024 ) {
    // allocate enough for all channels
    std::vector<float> floats( bufsize * from.channels() );

    sf_count_t read = 0;
    sf_count_t total_written = 0;
    auto sample_rate = from.samplerate();
    basic_envelope_generator gen( points, sample_rate, bufsize );

    while ( ( read = from.readf( floats.data(), bufsize ) ) ) {
        multichan_multiply( floats, gen.next_frames( read ), from.channels() );
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

static void normalize( std::vector<breakpoint::point> & points ) {
    auto max = max_point( points.begin(), points.end() );
    for ( auto x : points )
        x.value /= max.value;
}

static SndfileErr fwd_pan_copy( const std::string & from_path,
                                const std::string & to_path,
                                const std::string & breakpoints_path,
                                bool do_normalize ) {
    SndfileHandle from{ from_path, SFM_READ };
    if ( from.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open read file: " << from_path << std::endl;
        return SndfileErr::CouldNotOpen;
    }

    SndfileHandle to{ to_path, SFM_WRITE, from.format(), from.channels(), from.samplerate() };
    if ( to.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open write file: " << to_path << std::endl;
        return SndfileErr::CouldNotOpen;
    }

    auto breakpoints = breakpoint::parse_breakpoints( breakpoints_path );
    if ( auto * perr = std::get_if<breakpoint::parse_error>( &breakpoints ) ) {
        std::cout << "Error parsing breakpoint file '" << breakpoints_path << "': " << *perr
                  << std::endl;
        return SndfileErr::CouldNotOpen;
    } else if ( auto * pvals = std::get_if<std::vector<breakpoint::point>>( &breakpoints ) ) {
        if ( do_normalize )
            normalize( *pvals );
        return pan_copy( from, to, *pvals );
    } else {
        std::cout << "Unknown error while parsing breakpoints" << std::endl;
        return SndfileErr::CouldNotOpen;
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
        opts, std::bind( fwd_pan_copy, _1, _2, _3, opts.has( "normalize" ) ) );
}

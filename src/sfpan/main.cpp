#include <string>
#include <vector>

#include "breakpoint/breakpoint.hpp"
#include "util/checked_invoke.hpp"
#include "util/pan_utils.hpp"
#include "util/stereo_envelope_generator.hpp"

static void pan_multiply( std::vector<float> & out, const std::span<float> & in, sf_count_t read ) {
    while ( read-- != 0 ) {
        out[read * 2] = out[read] * in[read * 2];
        out[read * 2 + 1] = out[read] * in[read * 2 + 1];
    }
}

static bool check_pan_range( const std::vector<breakpoint::point> & points,
                             double min,
                             double max ) {
    return std::all_of( begin( points ), end( points ),
                        [min, max]( auto x ) { return x.value >= min && x.value <= max; } );
}

SndfileErr pan_copy( SndfileHandle & from,
                     SndfileHandle & to,
                     const std::vector<breakpoint::point> & points,
                     const size_t bufsize = 1024 ) {
    if ( !check_pan_range( points, -1.0, 1.0 ) ) {
        std::cout << "Breakpoints are outside the -1 to +1 range" << std::endl;
        return SndfileErr::BadOperation;
    }

    // allocate enough for stereo
    std::vector<float> floats( bufsize * 2 );

    sf_count_t read = 0;
    sf_count_t total_written = 0;
    auto sample_rate = from.samplerate();
    stereo_envelope_generator gen( points, sample_rate, bufsize );

    while ( ( read = from.readf( floats.data(), bufsize ) ) ) {
        pan_multiply( floats, gen.next_frames( read ), read );
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
                         const std::string & breakpoints_path ) {
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

    auto breakpoints = breakpoint::parse_breakpoints( breakpoints_path );
    if ( auto * perr = std::get_if<breakpoint::parse_error>( &breakpoints ) ) {
        std::cout << "Error parsing breakpoint file '" << breakpoints_path << "': " << *perr
                  << std::endl;
        return SndfileErr::CouldNotOpen;
    } else if ( auto * pvals = std::get_if<std::vector<breakpoint::point>>( &breakpoints ) ) {
        return pan_copy( from, to, *pvals );
    } else {
        std::cout << "Unknown error while parsing breakpoints" << std::endl;
        return SndfileErr::CouldNotOpen;
    }
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "sfpan", "Pan a mono file from a breakpoint file" };
    opts.basic_option( "help,h", "Print description and exit" )
        .positional( "input", "Input file" )
        .positional( "output", "Output file" )
        .positional( "breakpoints", "Breakpoint file" )
        .parse( argc, argv );

    return checked_invoke_in_out_bkpts( opts, &fwd_pan_copy );
}

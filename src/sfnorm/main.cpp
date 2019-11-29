#include <iostream>
#include <string>
#include <vector>

#include "util/checked_invoke.hpp"
#include "util/sndfile_utils.hpp"

bool get_peak_impl( SndfileHandle & handle, double & peak ) {
    if ( handle.command( SFC_GET_SIGNAL_MAX, &peak, sizeof( peak ) ) ) {
        return true;
    } else {

#ifndef NDEBUG
        std::cout << "Calculating normalized peak" << std::endl;
#endif

        int result = handle.command( SFC_CALC_NORM_SIGNAL_MAX, &peak, sizeof( peak ) );
        if ( result != 0 ) {
            std::cout << "Could not calc peak (return code=" << result << ")" << std::endl;
            return false;
        } else {
            return true;
        }
    }
}

static void warn_no_scale( Amplitude peak ) {
    std::cout << "Warning: new peak of " << peak << " requested, but no scaling will take place."
              << std::endl;
}

static bool get_peak( const std::string & input, double & peak ) noexcept {
    SndfileHandle in_handle{ input, SFM_READ };
    if ( in_handle.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open read file: " << input << std::endl;
        return false;
    }

    return get_peak_impl( in_handle, peak );
}

static bool print_peak( const std::string & input ) noexcept {
    double peak;
    auto ok = get_peak( input, peak );
    if ( ok ) {
        std::cout << amp_to_db( peak ) << std::endl;
    }

    return ok;
}

static bool normalize( const std::string & input,
                       const std::string & output,
                       double level_amp ) noexcept {
    SndfileHandle in_handle{ input, SFM_READ };
    if ( in_handle.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open read file: " << input << std::endl;
        return false;
    }

    double peak;
    if ( !get_peak_impl( in_handle, peak ) ) {
        return false;
    }

    const auto scale = level_amp / peak;

    if ( in_handle.seek( 0, SEEK_SET ) == -1 ) {
        std::cout << "Could not seek file: " << input << std::endl;
        return false;
    }

    if ( peak == level_amp ) {
        warn_no_scale( peak );
    }

    SndfileHandle out_handle{ output, SFM_WRITE, in_handle.format(), in_handle.channels(),
                              in_handle.samplerate() };
    if ( out_handle.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open write file: " << output << std::endl;
        return false;
    }

    return scale_copy( in_handle, out_handle, scale );
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "sfnorm", "Use peak information in a file to normalize it." };
    double level;
    opts.basic_option( "help,h", "Print description and exit" )
        .basic_option( "peak-only,p", "Print the peak in dB and exit" )
        .basic_option( "level,l", "Normalization level in dB",
                       simple_options::defaulted_value( &level, 0.0 ) )
        .positional( "input", "Input file" )
        .positional( "output", "Output file" )
        .parse( argc, argv );

    using namespace std::placeholders;
    if ( opts.has( "peak-only" ) ) {
        return checked_invoke( opts, std::array{ "input" }, print_peak );
    } else {
        return checked_invoke_in_out( opts, std::bind( normalize, _1, _2, db_to_amp( level ) ) );
    }
}

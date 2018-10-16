#include <iostream>
#include <string>
#include <vector>

#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

#include "sndfile.hh"

SndfileErr get_peak( SndfileHandle & handle, double & peak ) {
    if ( handle.command( SFC_GET_SIGNAL_MAX, &peak, sizeof( peak ) ) ) {
        return SndfileErr::Success;
    } else {

#ifndef NDEBUG
        std::cout << "Calculating normalized peak" << std::endl;
#endif

        int result = handle.command( SFC_CALC_NORM_SIGNAL_MAX, &peak, sizeof( peak ) );
        if ( result != 0 ) {
            std::cout << "Could not calc peak (return code=" << result << ")" << std::endl;
            return SndfileErr::BadOperation;
        } else {
            return SndfileErr::Success;
        }
    }
}

void print_peak( Amplitude peak ) {
    std::cout << amp_to_db( peak ) << std::endl;
}

void warn_no_scale( Amplitude peak ) {
    std::cout << "Warning: new peak of " << peak << " requested, but no scaling will take place."
              << std::endl;
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

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "input" ) && ( opts.has( "output" ) || opts.has( "peak-only" ) ) ) {
        auto && input = opts["input"].as<std::string>();
        SndfileHandle in_handle{ input, SFM_READ };
        if ( in_handle.error() != SF_ERR_NO_ERROR ) {
            std::cout << "Could not open read file: " << input << std::endl;
            return 1;
        }

        double peak;
        if ( get_peak( in_handle, peak ) != SndfileErr::Success ) {
            return 1;
        }
        auto level_amp = db_to_amp( level );
        auto scale = level_amp / peak;

        if ( in_handle.seek( 0, SEEK_SET ) == -1 ) {
            std::cout << "Could not seek file: " << input << std::endl;
            return 1;
        }

        if ( opts.has( "peak-only" ) ) {
            print_peak( peak );
            return 0;
        }

        if ( peak == level_amp ) {
            warn_no_scale( peak );
        }

        auto && output = opts["output"].as<std::string>();
        SndfileHandle out_handle{ output, SFM_WRITE, in_handle.format(), in_handle.channels(),
                                  in_handle.samplerate() };
        if ( out_handle.error() != SF_ERR_NO_ERROR ) {
            std::cout << "Could not open write file: " << output << std::endl;
            return 1;
        }

        if ( scale_copy( in_handle, out_handle, scale ) == SndfileErr::Success ) {
            return 0;
        } else {
            std::cout << "Copy failed." << std::endl;
        }
    } else {
        std::cout << opts;
        return 1;
    }

    return 0;
}

// simple things to help with sndfile stuff
#pragma once

#include "sndfile.hh"

#include <cmath>
#include <iostream>
#include <string>

std::ostream & operator<<( std::ostream & os, const SndfileHandle handle ) {
    os << "Format:      " << handle.format() << std::endl;
    os << "Frames:      " << handle.frames() << std::endl;
    os << "Channels:    " << handle.channels() << std::endl;
    os << "Sample rate: " << handle.samplerate() << std::endl;
    return os;
}

using Amplitude = float;
using Loudness = float;

Loudness amp_to_db( Amplitude amp ) {
    return 20.0 * std::log10( amp );
}

Amplitude db_to_amp( Loudness ld ) {
    return std::pow( 10.0, ld / 20.0 );
}

// Tries to open an input and output file, then forwards the resulting handles and other arguments
// to the given function. `F`'s type should be a callable `Ret ()( SndfileHandle&,
// SndfileHandle&, Ts... )`
template <typename F, typename... Ts>
bool fwd_copy( F && func,
               const std::string & from_path,
               const std::string & to_path,
               Ts &&... ts ) {
    SndfileHandle from{ from_path, SFM_READ };
    if ( from.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open read file: " << from_path << std::endl;
        return false;
    }

    SndfileHandle to{ to_path, SFM_WRITE, from.format(), from.channels(), from.samplerate() };
    if ( to.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open write file: " << to_path << std::endl;
        return false;
    }

#ifndef NDEBUG
    std::cout << from << "\n" << to << "\n";
#endif

    return func( from, to, std::forward<Ts &&>( ts )... );
}

bool scale_copy( SndfileHandle & from,
                 SndfileHandle & to,
                 const Amplitude amp_scale,
                 const size_t bufsize = 1024 ) {
    std::vector<float> floats( from.channels() * bufsize );

    sf_count_t read = 0;
    sf_count_t total_written = 0;
    while ( ( read = from.readf( floats.data(), bufsize ) ) ) {
        std::transform( floats.begin(), floats.begin() + ( read * from.channels() ), floats.begin(),
                        [&amp_scale]( auto x ) { return x * amp_scale; } );
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

// Factory methods
std::unique_ptr<SndfileHandle> make_input_handle( const std::string & path ) noexcept {
    auto handle = std::make_unique<SndfileHandle>( path, SFM_READ );
    if ( handle->error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open read file: " << path << std::endl;
        return {};
    }

    return handle;
}

std::unique_ptr<SndfileHandle> require_channels( std::unique_ptr<SndfileHandle> handle,
                                                 int chans ) noexcept {
    if ( !handle ) {
        return {};
    }

    if ( handle->channels() != chans ) {
        std::cout << "Input file must have " << chans << " channels (has: " << handle->channels()
                  << ")" << std::endl;
        return {};
    }

    return handle;
}

constexpr int SF_INPUT_FORMAT = 0;
constexpr int SF_INPUT_CHANNELS = -1;
std::unique_ptr<SndfileHandle> make_output_handle( const std::string & path,
                                                   const std::unique_ptr<SndfileHandle> & in_handle,
                                                   int format = SF_INPUT_FORMAT,
                                                   int chans = SF_INPUT_CHANNELS ) noexcept {
    if ( !in_handle ) {
        return {};
    }

    if ( format == SF_INPUT_FORMAT ) {
        format = in_handle->format();
    }

    if ( chans == SF_INPUT_CHANNELS ) {
        chans = in_handle->channels();
    }

    auto out_handle = std::make_unique<SndfileHandle>( path, SFM_WRITE, format, chans,
                                                       in_handle->samplerate() );
    if ( out_handle->error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open write file: " << path << std::endl;
        return {};
    }

    return out_handle;
}

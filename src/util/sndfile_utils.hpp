// simple things to help with sndfile stuff
#pragma once

#include "sndfile.hh"

#include <cmath>
#include <iostream>
#include <string>

enum class SndfileErr {
    Success = 0,
    CouldNotOpen,
    BadWrite,
    BadRead,
    BadOperation,
};

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
SndfileErr fwd_copy( F && func,
                     const std::string & from_path,
                     const std::string & to_path,
                     Ts &&... ts ) {
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

#ifndef NDEBUG
    std::cout << from << "\n" << to << "\n";
#endif

    return func( from, to, std::forward<Ts &&>( ts )... );
}

SndfileErr scale_copy( SndfileHandle & from,
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

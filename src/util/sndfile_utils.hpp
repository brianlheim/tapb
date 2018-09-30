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

Loudness ampToDb( Amplitude amp ) {
    return 20.0 * std::log10( amp );
}

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

    return func( from, to, std::forward<Ts &&...>( ts... ) );
}

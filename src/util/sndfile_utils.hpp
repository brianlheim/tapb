// simple things to help with sndfile stuff
#pragma once

#include "sndfile.hh"

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

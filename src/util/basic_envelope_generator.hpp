// pan iterator utilities
#pragma once

#include "util/envelope_generator_base.hpp"

// A general purpose breakpoint envelope generator.

class basic_envelope_generator : public envelop_generator_base<basic_envelope_generator, 1> {
    using Base = envelop_generator_base<basic_envelope_generator, 1>;
public:
    // Points: breakpoint envelope. Assumed to start from time 0. No assumption made about last
    // time. Sample rate: used for conversion from seconds to samples. Bufsize: max number of frames
    // that will be requested at one time.
    basic_envelope_generator( const breakpoint::point_list & points,
                              const uint32_t sample_rate,
                              const size_t bufsize ):
        Base(points, sample_rate, bufsize, 0.f) {
    }

private:
    friend Base;
    void generate_frames( std::vector<float>& buffer, const size_t index, const size_t n ) noexcept {
        for ( size_t i = 0; i < n; ++i )
            buffer[i] = val_at( index + i );
    }
};

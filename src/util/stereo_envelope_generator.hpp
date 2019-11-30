#pragma once

#include "util/envelope_generator_base.hpp"
#include "util/pan_utils.hpp"

// Represents a breakpoint envelope generator designed for multiplying with a stereo signal for
// panning.

class stereo_envelope_generator : public envelop_generator_base<stereo_envelope_generator, 2> {
    using Base = envelop_generator_base<stereo_envelope_generator, 2>;
public:
    // Points: breakpoint envelope. Assumed to start from time 0. No assumption made about last
    // time. Sample rate: used for conversion from seconds to samples. Bufsize: max number of frames
    // that will be requested at one time.
    stereo_envelope_generator( const breakpoint::point_list & points,
                               const uint32_t sample_rate,
                               const size_t bufsize ):
        Base( points, sample_rate, bufsize, audio_math::root_two_div_two() )
    {}

private:
    friend Base;
    void generate_frames( std::vector<float>& buffer, const size_t index, const size_t n ) noexcept {
        for ( size_t i = 0; i < n; ++i ) {
            auto val = val_at( index + i );
            auto pan = constant_power_pan<float>( val );
            buffer[i * 2] = pan.left;
            buffer[i * 2 + 1] = pan.right;
        }
    }
};

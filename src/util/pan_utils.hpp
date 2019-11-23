// panning utilities
#pragma once

#include "util/math_utils.hpp"

#include <cmath>

template <typename F> struct pan_pair {
    F left;
    F right;
};

template <typename F> pan_pair<F> constant_power_pan( double position ) {
    using namespace audio_math;

    const double angle = position * quarter_pi();
    const double sin_angle = std::sin( angle );
    const double cos_angle = std::cos( angle );

    return { F( root_two_div_two() * ( cos_angle - sin_angle ) ),
             F( root_two_div_two() * ( cos_angle + sin_angle ) ) };
}

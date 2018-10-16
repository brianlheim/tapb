// panning utilities
#pragma once

#include <cassert>
#include <cmath>
#include <tuple>

template <typename F>
struct pan_pair {
    F left;
    F right;
};

template <typename F> pan_pair<F> constant_power_pan( double position ) {
    const double pi_div_4 = std::atan( 1.0 );
    const double root2_div_2 = std::sqrt( 2.0 ) * 0.5;
    double angle = position * pi_div_4;
    double sin_angle = std::sin( angle );
    double cos_angle = std::cos( angle );

    return { root2_div_2 * ( cos_angle - sin_angle ), root2_div_2 * ( cos_angle + sin_angle ) };
}

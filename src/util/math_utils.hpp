// Common expressions
#pragma once

namespace audio_math {

constexpr double pi() {
    return 3.1415926535897932384;
}

constexpr double half_pi() {
    return pi() / 2.0;
}

constexpr double quarter_pi() {
    return pi() / 4.0;
}

constexpr double root_two() {
    return 1.4142135623730950488;
}

constexpr double root_two_div_two() {
    return root_two() / 2.0;
}

} // namespace audio_math

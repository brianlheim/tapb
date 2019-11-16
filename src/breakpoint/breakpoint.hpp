// Breakpoint utility library
#pragma once

#include <iosfwd>
#include <variant>
#include <vector>

namespace breakpoint {

struct point {
    double time_secs;
    double value;
};

struct parse_error {
    enum errc {
        success = 0,
        io_error = 1,
    };

    errc code;
    unsigned line; // starting from 1; 0 indicates nothing could be parsed at all
};

// File format:
//
// a series of lines, where each line is either (1) empty, or (2) a breakpoint.
// (1) empty: empty line with no whitespace
// (2) optional whitespace (spaces or tabs), fp time, whitespace, fp value, optional whitespace
//
// additional rules:
// - must be at least 2 breakpoints
// - first breakpoint must be at time 0.0
// - successive times must be increasing
std::variant<std::vector<point>, parse_error> parse_breakpoints( std::istream & is );

template <typename FwdIt> constexpr point max_point( FwdIt begin, FwdIt end ) {
    return *std::max_element(
        begin, end, []( const point & l, const point & r ) { return l.value < r.value; } );
}

} // namespace breakpoint

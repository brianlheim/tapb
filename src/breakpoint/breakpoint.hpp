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

constexpr bool operator==( point l, point r ) {
    return l.time_secs == r.time_secs && l.value == r.value;
}

constexpr bool operator!=( point l, point r ) {
    return !( l == r );
}

struct parse_error {
    enum errc {
        success = 0,
        io_error = 1,
        at_least_two_points,
        unexpected_eof, // if EOF does not come after a newline
        misformatted_line,
        line_too_long,
        time_not_increasing,
        first_time_not_zero,
    };

    errc code;
    unsigned line; // starting from 1; 0 indicates nothing could be parsed at all
};

const char * to_string( parse_error::errc code );

std::ostream & operator<<( std::ostream & os, const parse_error & error );
std::ostream & operator<<( std::ostream & os, parse_error::errc code );

// File format:
//
// a series of lines, where each line is either (1) empty, or (2) a breakpoint.
// whitespace = spaces or tabs
// (1) empty: empty line with optional whitespace
// (2) optional whitespace, fp time, whitespace, fp value, optional whitespace
//
// additional rules:
// - must be at least 2 breakpoints
// - first breakpoint must be at time 0.0
// - successive times must be increasing
// - line can be a maximum of 256 characters long
std::variant<std::vector<point>, parse_error> parse_breakpoints( std::istream & is );

// Helper function -- tries to open file at `path`, if it fails then return {io_error,0}
std::variant<std::vector<point>, parse_error> parse_breakpoints( const std::string & path );

template <typename FwdIt> constexpr point max_point( FwdIt begin, FwdIt end ) {
    return *std::max_element(
        begin, end, []( const point & l, const point & r ) { return l.value < r.value; } );
}

} // namespace breakpoint

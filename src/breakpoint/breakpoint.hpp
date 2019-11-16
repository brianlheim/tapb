// Breakpoint utility library
#pragma once

#include <iosfwd>
#include <vector>
#include <variant>

namespace breakpoint {

struct point {
    double time_secs;
    double value;
};

enum class bkpt_errc {
    success = 0,
    io_error = 1,
};

// invariant: if bkpt_errc == success, then .first contains the list of breakpoints from the file.
std::pair<std::vector<point>, bkpt_errc> parse_breakpoints( std::istream & is );

template <typename FwdIt> constexpr point max_point( FwdIt begin, FwdIt end ) {
    return *std::max_element( begin, end,
                             []( const point & l, const point & r ) { return l.value < r.value; } );
}

} // namespace breakpoint

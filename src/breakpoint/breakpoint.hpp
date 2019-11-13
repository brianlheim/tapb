// Breakpoint utility library
#pragma once

#include <iosfwd>
#include <vector>

namespace breakpoint {

struct point {
    double time_secs;
    double value;
};

enum class bkpt_errc {

};

// TODO error handling?
std::variant<std::vector<point>, bkpt_errc> parse_breakpoints( std::istream & is );

} // namespace breakpoint

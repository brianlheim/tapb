#include "breakpoint/breakpoint.hpp"

#include <istream>

namespace breakpoint {

std::pair<std::vector<point>, bkpt_errc> parse_breakpoints( std::istream & is ) {
    if ( !is )
        return { {}, bkpt_errc::io_error };

    std::vector<point> points;
    while ( true ) {
        point pt;
        is >> pt.time_secs >> pt.value;
        if ( ! is )
            break;
        points.push_back(pt);
    }

    if ( is.bad() )
        return { {}, bkpt_errc::io_error };
    else
        return { points, bkpt_errc::success };
}

} // namespace breakpoint


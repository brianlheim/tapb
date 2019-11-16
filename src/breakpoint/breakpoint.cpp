#include "breakpoint/breakpoint.hpp"

#include <istream>

namespace breakpoint {

std::variant<std::vector<point>, parse_error> parse_breakpoints( std::istream & is ) {
    (void)is;
    return { parse_error{ parse_error::io_error, 0 } };
}

} // namespace breakpoint

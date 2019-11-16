#include "breakpoint/breakpoint.hpp"

#include <charconv>
#include <istream>

namespace breakpoint {

static constexpr unsigned maxlen = 256u;

// Performs following validations:
// - should be at least two points
// - times should be increasing
// - first time should be 0.0
//
// Returns {error+line num} or {success+0}
static parse_error validate_breakpoints( const std::vector<point> & points,
                                         const std::vector<unsigned> & line_nums,
                                         unsigned last_line ) {
    assert( points.size() == line_nums.size() );

    if ( points.size() < 2 )
        return { parse_error::at_least_two_points, last_line };

    if ( points[0].time_secs != 0.0 )
        return { parse_error::first_time_not_zero, line_nums[0] };

    auto it = std::adjacent_find( begin( points ), end( points ),
                                  []( point l, point r ) { return l.time_secs >= r.time_secs; } );
    if ( it != end( points ) )
        return { parse_error::time_not_increasing, line_nums[it - points.begin()] };

    return { parse_error::success, 0 };
}

std::variant<std::vector<point>, parse_error> parse_breakpoints( std::istream & is ) {
    if ( !is )
        return { parse_error{ parse_error::io_error, 0 } };
    if ( is.peek() == std::istream::traits_type::eof() )
        return parse_error{ parse_error::unexpected_eof, 1 };

    unsigned line_count = 0;
    std::vector<point> result;
    std::vector<unsigned> line_nums;
    while ( true ) {
        if ( is.peek() == std::istream::traits_type::eof() )
            break;

        ++line_count;
        char line[maxlen + 1];
        is.getline( line, maxlen + 1 );
        if ( is.eof() )
            return parse_error{ parse_error::unexpected_eof, line_count };
        if ( is.fail() )
            return parse_error{ is.gcount() == maxlen ? parse_error::line_too_long
                                                      : parse_error::io_error,
                                line_count };

        // eat optional whitespace
        unsigned column = 0;
        while ( line[column] != '\0' && ( line[column] == ' ' || line[column] == '\t' ) )
            column++;

        if ( line[column] == '\0' )
            continue; // skip empty lines

        point this_point;
        char * time_end;
        this_point.time_secs = std::strtod( line + column, &time_end );
        if ( time_end == line + column || this_point.time_secs == HUGE_VAL )
            return parse_error{ parse_error::misformatted_line, line_count };
        column = time_end - line;

        while ( line[column] != '\0' && ( line[column] == ' ' || line[column] == '\t' ) )
            column++;

        if ( line + column == time_end )
            return parse_error{ parse_error::misformatted_line, line_count };

        char * value_end;
        this_point.value = std::strtod( line + column, &value_end );
        if ( value_end == line + column || this_point.value == HUGE_VAL )
            return parse_error{ parse_error::misformatted_line, line_count };
        column = value_end - line;

        while ( line[column] != '\0' && ( line[column] == ' ' || line[column] == '\t' ) )
            column++;

        if ( line[column] != '\0' )
            return parse_error{ parse_error::misformatted_line, line_count };

        result.push_back( this_point );
        line_nums.push_back( line_count );
    }

    auto && validate_error = validate_breakpoints( result, line_nums, line_count );
    using ReturnT = decltype( parse_breakpoints( is ) );
    return validate_error.code == parse_error::success ? ReturnT{ result } : validate_error;
}

} // namespace breakpoint

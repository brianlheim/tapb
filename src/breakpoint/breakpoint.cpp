#include "breakpoint/breakpoint.hpp"

#include <charconv>
#include <fstream>
#include <istream>
#include <ostream>

namespace breakpoint {

const char * to_string( parse_error::errc code ) {
    switch ( code ) {
    case parse_error::success:
        return "Success";
    case parse_error::io_error:
        return "I/O error";
    case parse_error::at_least_two_points:
        return "Need at least two points";
    case parse_error::unexpected_eof:
        return "Unexpected end of file";
    case parse_error::misformatted_line:
        return "Misformatted line";
    case parse_error::line_too_long:
        return "Line too long";
    case parse_error::time_not_increasing:
        return "Times must increase";
    case parse_error::first_time_not_zero:
        return "First time must be zero";
    default:
        return "Unknown error";
    }
}

std::ostream & operator<<( std::ostream & os, const parse_error & error ) {
    return os << error.code << " (line " << error.line << ')';
}

std::ostream & operator<<( std::ostream & os, parse_error::errc code ) {
    return os << to_string( code );
}

// Max line length
static constexpr unsigned maxlen = 256u;

// Performs following validations:
// - should be at least two points
// - times should be increasing
// - first time should be 0.0
//
// Returns {error+line num} or {success+0}
static parse_error validate_breakpoints( const point_list & points,
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

// Returns whether or not parsing was successful
// If successful, column is set to past end of double, out is set to double value
static bool try_parse_double( const char *& column, double & out ) {
    char * str_end;
    out = std::strtod( column, &str_end );
    if ( str_end == column || out == HUGE_VAL )
        return false;
    column = str_end;
    return true;
}

// Eat up spaces and tabs
static inline const char * scan_to_next_token( const char * column ) {
    while ( *column != '\0' && ( *column == ' ' || *column == '\t' ) )
        column++;
    return column;
}

std::variant<point_list, parse_error> parse_breakpoints( std::istream & is ) {
    if ( !is )
        return { parse_error{ parse_error::io_error, 0 } };
    if ( is.peek() == std::istream::traits_type::eof() )
        return parse_error{ parse_error::unexpected_eof, 1 };

    unsigned line_count = 0;
    point_list result;
    std::vector<unsigned> line_nums;
    while ( true ) {
        // Only successful exit path from this function
        if ( is.peek() == std::istream::traits_type::eof() ) {
            auto && validate_error = validate_breakpoints( result, line_nums, line_count );
            using ReturnT = decltype( parse_breakpoints( is ) );
            return validate_error.code == parse_error::success ? ReturnT{ result } : validate_error;
        }

        ++line_count;
        char line[maxlen + 1];
        is.getline( line, maxlen + 1 );
        if ( is.eof() )
            return parse_error{ parse_error::unexpected_eof, line_count };
        if ( is.fail() )
            return parse_error{ is.gcount() == maxlen ? parse_error::line_too_long
                                                      : parse_error::io_error,
                                line_count };

        // Process each line
        auto * column = scan_to_next_token( line );
        if ( *column == '\0' )
            continue; // skip empty lines

        // Time
        point this_point;
        if ( !try_parse_double( column, this_point.time_secs ) )
            return parse_error{ parse_error::misformatted_line, line_count };

        auto * next = scan_to_next_token( column );

        if ( column == next )
            return parse_error{ parse_error::misformatted_line, line_count };
        column = next;

        // Value
        if ( !try_parse_double( column, this_point.value ) )
            return parse_error{ parse_error::misformatted_line, line_count };

        column = scan_to_next_token( column );

        if ( *column != '\0' )
            return parse_error{ parse_error::misformatted_line, line_count };

        // Success
        result.push_back( this_point );
        line_nums.push_back( line_count );
    }
}

std::variant<point_list, parse_error> parse_breakpoints( const std::string & path ) {
    std::ifstream ifs{ path };
    return ifs.is_open() ? parse_breakpoints( ifs ) : parse_error{ parse_error::io_error, 0 };
}

bool write_breakpoints( std::ostream & os, const point_list & points ) {
    for ( auto & point : points )
        os << point.time_secs << '\t' << point.value << '\n';
    return bool( os );
}

bool write_breakpoints( const std::string & path, const point_list & points ) {
    std::ofstream ofs{ path };
    return ofs.is_open() && write_breakpoints( ofs, points );
}

} // namespace breakpoint

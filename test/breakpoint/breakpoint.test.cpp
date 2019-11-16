#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "breakpoint/breakpoint.hpp"

#include <ios>
#include <sstream>

using namespace breakpoint;

static void require_error( std::istream & is, parse_error::errc code, unsigned line ) {
    auto result = breakpoint::parse_breakpoints( is );
    auto * parse_res = std::get_if<breakpoint::parse_error>( &result );
    REQUIRE( parse_res );
    REQUIRE( parse_res->line == line );
    REQUIRE( parse_res->code == code );
}

static void require_error( const std::string & input, parse_error::errc code, unsigned line ) {
    require_error( std::istringstream( input ), code, line );
}

TEST_CASE( "Cannot parse bad istream" ) {
    std::istringstream is;
    is.setstate( std::ios_base::failbit );
    require_error( is, parse_error::io_error, 0 );
}

TEST_CASE( "Empty str should fail with i/o error (no final newline)" ) {
    require_error( "", parse_error::io_error, 1 );
}

TEST_CASE( "Empty line should fail with need 2+ breakpoints" ) {
    require_error( "\n", parse_error::at_least_two_points, 1 );
}

TEST_CASE( "Empty line should fail with need 2+ breakpoints" ) {
    require_error( "\n", parse_error::at_least_two_points, 1 );
}

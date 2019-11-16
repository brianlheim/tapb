#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "breakpoint/breakpoint.hpp"

#include <ios>
#include <sstream>

TEST_CASE( "Cannot parse bad istream" ) {
    std::istringstream is;
    is.setstate( std::ios_base::failbit );
    auto result = breakpoint::parse_breakpoints( is );
    auto * parse_res = std::get_if<breakpoint::parse_error>( &result );
    REQUIRE( parse_res );
    REQUIRE( parse_res->line == 0 );
    REQUIRE( parse_res->code == breakpoint::parse_error::io_error );
}

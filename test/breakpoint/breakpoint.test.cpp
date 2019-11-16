#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "breakpoint/breakpoint.hpp"

#include <ios>
#include <sstream>

using namespace breakpoint;
using namespace Catch;

static void require_error( std::istringstream & is, parse_error::errc code, unsigned line ) {
    auto result = parse_breakpoints( is );
    auto * error = std::get_if<parse_error>( &result );
    INFO( "Input[" << is.str() << "]" );
    REQUIRE( error );
    CHECK( error->line == line );
    CHECK( error->code == code );
}

static void require_error( const std::string & input, parse_error::errc code, unsigned line ) {
    std::istringstream is( input );
    require_error( is, code, line );
}

static void require_success( const std::string & input, const std::vector<point> & exp_points ) {
    std::istringstream is( input );
    auto result = parse_breakpoints( is );
    auto * points = std::get_if<std::vector<point>>( &result );
    INFO( "Input[" << input << "]" );
    REQUIRE( points );
    CHECK_THAT( *points, Equals( exp_points ) );
}

TEST_CASE( "Cannot parse bad istream" ) {
    std::istringstream is;
    is.setstate( std::ios_base::failbit );
    require_error( is, parse_error::io_error, 0 );
}

TEST_CASE( "Empty str should fail with unexpected eof" ) {
    require_error( "", parse_error::unexpected_eof, 1 );
}

TEST_CASE( "Empty line should fail with need 2+ breakpoints" ) {
    require_error( "\n", parse_error::at_least_two_points, 1 );
}

TEST_CASE( "Whitespace empty line should need 2+ breakpoints" ) {
    require_error( "         \n", parse_error::at_least_two_points, 1 );
}

TEST_CASE( "Misformatted first line" ) {
    require_error( "0.0\n", parse_error::misformatted_line, 1 );
    require_error( "  0.0\n", parse_error::misformatted_line, 1 );
    require_error( "\t0.0\n", parse_error::misformatted_line, 1 );
    require_error( "0.0  \n", parse_error::misformatted_line, 1 );
    require_error( "0.0\t\n", parse_error::misformatted_line, 1 );
    require_error( "0.0\tX\n", parse_error::misformatted_line, 1 );
    require_error( "X\n", parse_error::misformatted_line, 1 );
    require_error( "0.0\t0.0\tX\n", parse_error::misformatted_line, 1 );
    require_error( "0.0\t0.0\t0.0\n", parse_error::misformatted_line, 1 );
    require_error( "0.00.0\n", parse_error::misformatted_line, 1 );
    require_error( "00\n", parse_error::misformatted_line, 1 );
}

TEST_CASE( "Only one point" ) {
    // Should all be OK except for at_least_two_points
    require_error( "0.0 0.0\n", parse_error::at_least_two_points, 1 );
    require_error( " 0.0 0.0\n", parse_error::at_least_two_points, 1 );
    require_error( " 0.0 0.0 \n", parse_error::at_least_two_points, 1 );
    require_error( "\t0.0\t0.0\t\n", parse_error::at_least_two_points, 1 );
    require_error( "0.0 0.0\n", parse_error::at_least_two_points, 1 );
    require_error( "0 0\n", parse_error::at_least_two_points, 1 );
    require_error( "0.0 0\n", parse_error::at_least_two_points, 1 );
    require_error( "0.0 -0\n", parse_error::at_least_two_points, 1 );
    require_error( "0.0 -0.0\n", parse_error::at_least_two_points, 1 );
}

TEST_CASE( "Only one point one blank line" ) {
    // Should be error on line 2
    require_error( "\n0.0 0.0\n", parse_error::at_least_two_points, 2 );
}

TEST_CASE( "success simple" ) {
    require_success( "0.0 0.0\n1.0 1.0\n", { { 0, 0 }, { 1, 1 } } );
    require_success( "0 0\n1 1\n", { { 0, 0 }, { 1, 1 } } );
    require_success( "0 0\n1 -1\n", { { 0, 0 }, { 1, -1 } } );
}

TEST_CASE( "success empty lines" ) {
    require_success( "\n0 0\n\n1 1\n\n", { { 0, 0 }, { 1, 1 } } );
    require_success( " \n 0 0 \n  \n1 1 \n\n", { { 0, 0 }, { 1, 1 } } );
}

TEST_CASE( "success long line" ) {
    std::string line1( 256, ' ' );
    std::string line2 = line1;
    line1[0] = line1[2] = '0';
    line2[0] = line2[2] = '1';
    require_success( line1 + '\n' + line2 + '\n', { { 0, 0 }, { 1, 1 } } );
}

TEST_CASE( "failure line too long" ) {
    std::string line1( 256, ' ' );
    std::string line2 = line1;
    line1[0] = line1[2] = '0';
    line2[0] = line2[2] = '1';
    require_error( line1 + '\n' + line2 + " \n", parse_error::line_too_long, 2 );
}

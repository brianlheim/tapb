#include <fstream>
#include <iostream>
#include <string>

#include "breakpoint/breakpoint.hpp"
#include "util/checked_invoke.hpp"

static bool process_breakpoints_file( const std::string & infile ) noexcept {
    auto result = breakpoint::parse_breakpoints( infile );
    auto * err = std::get_if<breakpoint::parse_error>( &result );
    if ( err ) {
        std::cout << "Error " << err->code << " on line: " << err->line << std::endl;
        return false;
    }

    auto & points = *std::get_if<breakpoint::point_list>( &result );
    for ( auto && pt : points ) {
        std::cout << "Point: " << pt.time_secs << " @ " << pt.value << std::endl;
    }

    auto && max_pt = breakpoint::max_point( begin( points ), end( points ) );
    std::cout << "\nMax point: " << max_pt.time_secs << " @ " << max_pt.value << std::endl;
    return true;
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "bkpts_driver" };
    opts.basic_option( "help,h", "Print description and exit" )
        .positional( "file", "Input file" )
        .parse( argc, argv );

    return checked_invoke( opts, std::array{ "file" }, &process_breakpoints_file );
}

#include <iostream>
#include <string>
#include <fstream>

#include "breakpoint/breakpoint.hpp"
#include "util/simple_options.hpp"

int main( int argc, char ** argv ) {
    simple_options::options opts{ "bkpts_driver" };
    opts.basic_option( "help,h", "Print description and exit" )
        .positional( "file", "Input file" )
        .parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "file" ) ) {
        std::ifstream infile( opts[ "file" ].as<std::string>() );
        auto&& [points, errc] = breakpoint::parse_breakpoints( infile );
        if ( errc != breakpoint::bkpt_errc::success )
        {
            std::cout << "Error while parsing breakpoints: " << std::underlying_type_t<breakpoint::bkpt_errc>(errc) << std::endl;
            return 1;
        }
        else
        {
            for ( auto&& pt : points )
            {
                std::cout << "Point: " << pt.time_secs << " @ " << pt.value << std::endl;
            }

            auto&& max_pt = breakpoint::max_point( begin(points), end( points ) );
            std::cout << "\nMax point: " << max_pt.time_secs << " @ " << max_pt.value << std::endl;
        }
    }
    else
    {
        std::cout << opts;
        return 1;
    }

    return 0;
}

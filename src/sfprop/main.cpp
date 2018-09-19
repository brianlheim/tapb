// take one file and randomly write its frames to two output files
#include <iostream>

#include "wrapsndfile.hpp"

#include "util/simple_options.hpp"

int main( int argc, char ** argv ) {
    simple_options::options opts{ "sfprop" };
    opts.basic_option( "help,h", "Print description and exit" )
        .positional( "input", "Input file" )
        .parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "input" ) ) {
        print_properties( opts[ "input" ].as<std::string>() );
    } else {
        std::cout << opts;
        return 1;
    }

    return 0;
}

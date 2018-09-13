#include <iostream>

#include "simple_options.hpp"

int main( int argc, char ** argv ) {
    simple_options::options opts{};
    opts.option( "help,h", "Print description and exit" ).positional( "file" ).parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "file" ) ) {
    }

    return 0;
}

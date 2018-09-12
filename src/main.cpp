#include <iostream>

#include "args.hpp"

int main( int argc, char ** argv )
{
    options::options opts{ argc, argv };
    if ( opts.count( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.count( "file" ) ) {

    }

    return 0;
}

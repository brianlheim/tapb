#include <iostream>
#include <string>

#include "simple_options.hpp"

int main( int argc, char ** argv ) {
    simple_options::options opts{ "my_program" };
    opts.option( "help,h", "Print description and exit" )
        .positional( "file", "Input file" )
        .parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "file" ) ) {
        std::cout << "File is " << opts["file"].as<std::string>() << std::endl;
    }

    return 0;
}

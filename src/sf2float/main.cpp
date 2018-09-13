#include <iostream>
#include <string>

#include "util/simple_options.hpp"

bool do_copy( std::string, std::string ) {
    return true;
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "my_program" };
    opts.basic_option( "help,h", "Print description and exit" )
        .positional( "input", "Input file" )
        .positional( "output", "Input file" )
        .parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "input" ) && opts.has( "output" ) ) {
        if ( do_copy( opts["input"].as<std::string>(), opts["output"].as<std::string>() ) ) {
            return 0;
        } else {
            std::cout << "Copy failed." << std::endl;
        }
    } else {
        std::cout << opts;
        return 1;
    }

    return 0;
}

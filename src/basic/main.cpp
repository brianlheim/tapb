#include <iostream>
#include <string>

#include "util/simple_options.hpp"

int main( int argc, char ** argv ) {
    int val;
    std::string name;

    simple_options::options opts{ "my_program" };
    opts.basic_option( "help,h", "Print description and exit" )
        .stored_option( "value,v", "A value", &val )
        .stored_option( "name,n", "A name", &name )
        .positional( "file", "Input file" )
        .parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "value" ) ) {
        std::cout << "Value is " << val << std::endl;
    }

    if ( opts.has( "name" ) ) {
        std::cout << "Name is " << name << std::endl;
    }

    if ( opts.has( "file" ) ) {
        std::cout << "File is " << opts["file"].as<std::string>() << std::endl;
    }

    return 0;
}

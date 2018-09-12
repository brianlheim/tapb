#pragma once

#include <iostream>

namespace options {

class options {
public:
    options( int argc, char ** argv );

    int count( const char * arg ) const;

    friend std::ostream & operator<<( std::ostream & os, const options & options );

private:
    struct impl;
    impl * _pimpl;
};

} // namespace options

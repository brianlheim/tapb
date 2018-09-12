#pragma once

#include <iostream>

namespace options {

class options {
public:
    options( int argc, char ** argv );

    int count( const char * arg ) const;

    friend std::ostream & operator<<( std::ostream & os, const options & options );

private:
    auto create_desc() const;
    auto create_pos_desc() const;
    auto parse_cmd_line( int argc, char ** argv );

    struct impl;
    impl * _pimpl;
};

} // namespace options

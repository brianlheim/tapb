#pragma once

#include <boost/program_options.hpp>

#include <iostream>

namespace boost::program_options {
class options_description;
class positional_options_description;
class variables_map;
} // namespace boost::program_options

namespace options {

class options {
public:
    using options_desc = boost::program_options::options_description;
    using pos_options_desc = boost::program_options::positional_options_description;
    using variables_map = boost::program_options::variables_map;

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

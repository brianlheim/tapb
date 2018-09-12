#pragma once

#include <boost/program_options.hpp>

#include <iostream>

namespace options {

class options {
public:
    using options_desc = boost::program_options::options_description;
    using pos_options_desc = boost::program_options::positional_options_description;
    using variables_map = boost::program_options::variables_map;

    options( int argc, char** argv );

    int count( const char * arg ) const;

    friend std::ostream& operator<<( std::ostream& os, const options & options );

private:
    const auto create_desc() const;
    const auto create_pos_desc() const;
    const auto parse_cmd_line( int argc, char ** argv );

    const options_desc _desc;
    const pos_options_desc _pos_desc;
    const variables_map _vm;
};

}

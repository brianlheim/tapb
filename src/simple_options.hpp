#pragma once

// boost::program_options's API blows
#include <boost/program_options.hpp>

namespace simple_options {

class options : public boost::program_options::variables_map {
public:
    using desc_t = boost::program_options::options_description;
    using posl_t = boost::program_options::positional_options_description;
    using semantic_t = boost::program_options::value_semantic;
    using map_t = options;

    options() = default;
    ~options() noexcept = default;

    /// Add a simple option with name and description.
    options & option( const char * name, const char * description ) {
        _desc.add_options()( name, description );
        return *this;
    }

    /// See value<>() in boost::program_options for info on creating semantics
    options & option( const char * name, const semantic_t * semantic, const char * description ) {
        _desc.add_options()( name, semantic, description );
        return *this;
    }

    /// Add a positional argument with an unlimited number of slots.
    options & unlimited_positional( const char * name ) {
        _posl.add( name, -1 );
        return *this;
    }

    /// Add a positional argument with a specified number of slots.
    options & positional( const char * name, int max_count = 1 ) {
        _posl.add( name, max_count );
        return *this;
    }

    void parse( int argc, char ** argv ) {
        auto && result = boost::program_options::command_line_parser( argc, argv )
                             .options( _desc )
                             .positional( _posl )
                             .run();
        store( result, *this );
    }

    bool has( std::string const & field ) const {
        return count( field ) != 0;
    }

    friend std::ostream & operator<<( std::ostream & os, options & opts ) {
        os << opts._desc;
        return os;
    }

private:
    desc_t _desc;
    posl_t _posl;
};

}

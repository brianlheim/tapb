#pragma once

// boost::program_options's API blows
#include <boost/program_options.hpp>

#include <optional>

namespace simple_options {

class options : public boost::program_options::variables_map {
public:
    using desc_t = boost::program_options::options_description;
    using posl_t = boost::program_options::positional_options_description;
    using semantic_t = boost::program_options::value_semantic;
    using map_t = options;

    options() = default;
    options( std::string const & name ): _desc{}, _posl{}, _name{ name } {
    }

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
    options & unlimited_positional( const char * name, const char * description ) {
        positional( name, description, -1 );
        return *this;
    }

    /// Add a positional argument with a specified number of slots.
    options & positional( const char * name, const char * description, int max_count = 1 ) {
        _posl.add( name, max_count );
        _desc_pos.add_options()( name, description );
        update_posl_count( max_count );
        return *this;
    }

    void parse( int argc, char ** argv ) {
        desc_t full_desc{};
        full_desc.add( _desc ).add( _desc_pos );
        auto && result = boost::program_options::command_line_parser( argc, argv )
                             .options( full_desc )
                             .positional( _posl )
                             .run();
        store( result, *this );
    }

    bool has( std::string const & field ) const {
        return count( field ) != 0;
    }

    friend std::ostream & operator<<( std::ostream & os, options & opts ) {
        opts.print_on( os );
        return os;
    }

private:
    void print_on( std::ostream & os ) const {
        os << "Usage:\n    " << *_name;
        for ( auto i = 0u; i < _pos_count; ++i ) {
            os << " <" << _posl.name_for_position( i ) << ">";
        }

        // Has an unlimited parameter
        if ( _posl.max_total_count() > _pos_count ) {
            os << " <" << _posl.name_for_position( _pos_count + 1 ) << "...>";
        }

        os << "\n\n";
        os << _desc;
    }

    void update_posl_count( int count ) {
        // ignore -1 == unlimited
        if ( count >= 0 ) {
            _pos_count += count;
        }
    }

    desc_t _desc;
    desc_t _desc_pos; // Invisible positional arguments
    posl_t _posl;
    unsigned _pos_count; // Num non-unlimited positional arguments
    const std::optional<std::string> _name;
};

}

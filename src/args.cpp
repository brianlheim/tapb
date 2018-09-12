#include "args.hpp"
#include "simple_options.hpp"

namespace options {

struct options::impl {
    simple_options::options _opts;

    impl( int argc, char ** argv ) {
        _opts.option( "h,help", "Print help and exit" );
        _opts.positional( "file" );
        _opts.parse( argc, argv );
    }
};

options::options( int argc, char ** argv ): _pimpl{ new impl{ argc, argv } } {
}

int options::count( const char * arg ) const {
    return _pimpl->_opts.count( arg );
}

std::ostream & operator<<( std::ostream & os, const options & options ) {
    os << options._pimpl->_opts << std::endl;
    return os;
}

} // namespace options

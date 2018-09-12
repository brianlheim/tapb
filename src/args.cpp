#include "args.hpp"

#include <boost/program_options.hpp>

namespace options {

auto options::create_desc() const {
    options_desc desc;
    desc.add_options()( "h,help", "Print help and exit" )
        // ( "v,verbose", "Verbose messages" )
        ( "f,file", "Target file" );
    return desc;
}

auto options::create_pos_desc() const {
    pos_options_desc pod;
    pod.add( "f,file", 1 );
    return pod;
}

options::options( int /*argc*/, char ** /*argv*/ ):
    _desc{ create_desc() },
    _vm{} {
}

int options::count( const char * arg ) const {
    return _vm.count( arg );
}

std::ostream & operator<<( std::ostream & os, const options & options ) {
    os << options._desc << std::endl;
    return os;
}

} // namespace options

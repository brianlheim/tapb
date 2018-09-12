#include "args.hpp"

#include <boost/program_options.hpp>

namespace options {

using options_desc = boost::program_options::options_description;
using pos_options_desc = boost::program_options::positional_options_description;
using variables_map = boost::program_options::variables_map;

struct options::impl {
    const options_desc _desc;
    const pos_options_desc _pos_desc;
    const variables_map _vm;
};

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
    _pimpl{ new impl{} } {
}

int options::count( const char * arg ) const {
    return _pimpl->_vm.count( arg );
}

std::ostream & operator<<( std::ostream & os, const options & options ) {
    os << options._pimpl->_desc << std::endl;
    return os;
}

} // namespace options

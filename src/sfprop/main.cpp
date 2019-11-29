// print out properties of a sound file
#include <filesystem>
#include <iostream>
#include <string>

#include "wrapsndfile.hpp"

#include "util/checked_invoke.hpp"

using namespace std::string_literals;

using SndfileValuePropertyFun = bool ( WrapSndfile::sndfile::* )( double & );

std::string try_get_value_property( WrapSndfile::sndfile & sf,
                                    const SndfileValuePropertyFun func ) {
    double val = 0.0;
    if ( ( sf.*func )( val ) ) {
        return std::to_string( val ) + " (" + std::to_string( amp_to_db( val ) ) + " dB)";
    } else {
        return "not found"s;
    }
}

static SndfileErr print_properties( const std::string & path ) noexcept {
    WrapSndfile::sndfile sf( path, SFM_READ );
    if ( !sf ) {
        std::cout << "Couldn't open file: " << path;
        return SndfileErr::CouldNotOpen;
    }

    using std::cout, std::endl;
    const std::string divider( 60, '-' );

    cout << "Filename:        " << path << endl;
    cout << divider << endl;
    cout << "Format:          " << WrapSndfile::formatTypeName( sf.format().type ) << endl;
    cout << "Subtype:         " << WrapSndfile::formatSubtypeName( sf.format().subtype ) << endl;
    cout << "Endianness:      " << sf.format().endianness << endl;
    cout << "Channels:        " << sf.channels() << endl;
    cout << "Frames:          " << sf.frames() << endl;
    cout << "Sample Rate:     " << sf.samplerate() << endl;
    cout << "In-file Peak:    " << try_get_value_property( sf, &WrapSndfile::sndfile::getPeak )
         << endl;
    cout << "Calculated Peak: " << try_get_value_property( sf, &WrapSndfile::sndfile::calcPeak )
         << endl;
    cout << "Calc Norm Peak:  "
         << try_get_value_property( sf, &WrapSndfile::sndfile::calcNormalizedPeak ) << endl;
    cout << divider << '\n' << endl;

    auto const fields = {
        WrapSndfile::Field::TITLE,  WrapSndfile::Field::COPYRIGHT, WrapSndfile::Field::SOFTWARE,
        WrapSndfile::Field::ARTIST, WrapSndfile::Field::COMMENT,   WrapSndfile::Field::DATE,
        WrapSndfile::Field::ALBUM,  WrapSndfile::Field::LICENSE,   WrapSndfile::Field::TRACKNUMBER,
        WrapSndfile::Field::GENRE,
    };

    for ( auto const field : fields ) {
        if ( std::string s; sf.getField( field, s ) ) {
            cout << field << ": " << s << endl;
        }
    }

    return SndfileErr::Success;
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "sfprop" };
    opts.basic_option( "help,h", "Print description and exit" )
        .positional( "input", "Input file" )
        .parse( argc, argv );

    return checked_invoke( opts, std::array{ "input" }, &print_properties );
}

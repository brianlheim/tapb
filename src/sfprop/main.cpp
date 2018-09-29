// print out properties of a sound file
#include <filesystem>
#include <iostream>
#include <string>

#include "wrapsndfile.hpp"

#include "util/simple_options.hpp"
#include "util/sndfile_utils.hpp"

using namespace std::string_literals;

using SndfileValuePropertyFun = bool ( WrapSndfile::sndfile::* )( double & );

std::string try_get_value_property( WrapSndfile::sndfile & sf, const SndfileValuePropertyFun func ) {
    double val = 0.0;
    if ( ( sf.*func )( val ) ) {
        return std::to_string( val ) + " (" + std::to_string( ampToDb( val ) ) + " dB)";
    } else {
        return "not found"s;
    }
}

void print_properties( const std::filesystem::path & path, WrapSndfile::sndfile & sf ) {
    using std::cout, std::endl;

    cout << "Filename:        " << path.filename() << endl;
    cout << "------------------------------------------------------" << endl;
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

    auto const fields = {
        WrapSndfile::Field::TITLE,
        WrapSndfile::Field::COPYRIGHT,
        WrapSndfile::Field::SOFTWARE,
        WrapSndfile::Field::ARTIST,
        WrapSndfile::Field::COMMENT,
        WrapSndfile::Field::DATE,
        WrapSndfile::Field::ALBUM,
        WrapSndfile::Field::LICENSE,
        WrapSndfile::Field::TRACKNUMBER,
        WrapSndfile::Field::GENRE,
    };

    for ( auto const field : fields ) {
        if ( std::string s; sf.getField( field, s ) ) {
            cout << field << ": " << s << endl;
        }
    }
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "sfprop" };
    opts.basic_option( "help,h", "Print description and exit" )
        .positional( "input", "Input file" )
        .parse( argc, argv );

    if ( opts.has( "help" ) ) {
        std::cout << opts;
        return 0;
    }

    if ( opts.has( "input" ) ) {
        auto & path = opts["input"].as<std::string>();
        WrapSndfile::sndfile sf( path, SFM_READ );
        if ( sf ) {
            print_properties( path, sf );
        } else {
            std::cout << "Couldn't open file" << std::endl;
        }
    } else {
        std::cout << opts;
        return 1;
    }

    return 0;
}

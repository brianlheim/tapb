// print out properties of a sound file
#include <filesystem>
#include <iostream>

#include "wrapsndfile.hpp"

#include "util/simple_options.hpp"

void print_properties( const std::filesystem::path & path ) {
    using std::cout, std::endl;

    WrapSndfile::sndfile sf( path.c_str(), SFM_READ );

    if ( sf ) {
        cout << "Filename:        " << path.filename() << endl;
        cout << "------------------------------------------------------" << endl;
        cout << "Format:          " << WrapSndfile::formatTypeName( sf.format().type ) << endl;
        cout << "Subtype:         " << WrapSndfile::formatSubtypeName( sf.format().subtype )
             << endl;
        cout << "Endianness:      " << sf.format().endianness << endl;
        cout << "Channels:        " << sf.channels() << endl;
        cout << "Frames:          " << sf.frames() << endl;
        cout << "Sample Rate:     " << sf.samplerate() << endl;
        cout << "In-file Peak:    ";
        {
            double peak = 3;
            if ( sf.getPeak( peak ) ) {
                cout << peak << endl;
            } else {
                cout << "not found" << endl;
            }
        }

        cout << "Calculated Peak: ";
        {
            double peak;
            if ( sf.calcPeak( peak ) ) {
                cout << peak << endl;
            } else {
                cout << "not found" << endl;
            }
        }

        cout << "Calc Norm Peak:  ";
        {
            double peak;
            if ( sf.calcNormalizedPeak( peak ) ) {
                cout << peak << endl;
            } else {
                cout << "not found" << endl;
            }
        }
    } else {
        cout << "Couldn't open :(" << endl;
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
        print_properties( opts["input"].as<std::string>() );
    } else {
        std::cout << opts;
        return 1;
    }

    return 0;
}

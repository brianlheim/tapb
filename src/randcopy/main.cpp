// take one file and randomly write its frames to two output files
#include <cstdlib> // for rand()
#include <iostream>
#include <string>
#include <vector>

#include "util/checked_invoke.hpp"
#include "util/sndfile_utils.hpp"

#include "sndfile.hh"

bool do_copy_impl( SndfileHandle & from,
                   SndfileHandle & to1,
                   SndfileHandle & to2,
                   const size_t bufsize,
                   const size_t randblock ) {
    std::vector<float> floats( from.channels() * bufsize );
    auto choose_new_outfile = [&to1, &to2]() -> SndfileHandle { return rand() & 0x1 ? to1 : to2; };

    sf_count_t read = 0;
    sf_count_t total_written = 0;
    sf_count_t to_write_in_block = randblock;
    auto outfile = choose_new_outfile();

    while ( ( read = from.readf( floats.data(), bufsize ) ) ) {
        sf_count_t written = 0;
        while ( written < read ) {
            if ( to_write_in_block == 0 ) {
                outfile = choose_new_outfile();
                to_write_in_block = randblock;
            }

            auto write_size = std::min( to_write_in_block, read - written );
            auto written_this_pass
                = outfile.writef( floats.data() + written * from.channels(), write_size );

            if ( written_this_pass < write_size ) {
                std::cout << "Error while writing (" << written_this_pass
                          << " written): " << outfile.strError() << std::endl;
                return false;
            }

            to_write_in_block -= write_size;
            written += written_this_pass;
        }

        total_written += written;
    }

    if ( total_written != from.frames() ) {
        std::cout << "Could not read entire file: " << from.strError() << std::endl;
        std::cout << "Read " << from.frames() << " | Wrote " << to1.frames() << ", " << to2.frames()
                  << std::endl;
        return false;
    }

    return true;
}

bool do_copy( const std::string & from_path,
              const std::string & to_path1,
              const std::string & to_path2,
              const size_t bufsize,
              const size_t randblock ) {
    SndfileHandle from{ from_path, SFM_READ };
    if ( from.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open read file: " << from_path << std::endl;
        return false;
    }

    SndfileHandle to1{ to_path1, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, from.channels(),
                       from.samplerate() };
    if ( to1.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open write file: " << to_path1 << " (" << to1.strError() << ")"
                  << std::endl;
        return false;
    }

    SndfileHandle to2{ to_path2, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, from.channels(),
                       from.samplerate() };
    if ( to2.error() != SF_ERR_NO_ERROR ) {
        std::cout << "Could not open write file: " << to_path2 << " (" << to2.strError() << ")"
                  << std::endl;
        return false;
    }

#ifndef NDEBUG
    std::cout << from << "\n" << to1 << "\n" << to2;
#endif

    return do_copy_impl( from, to1, to2, bufsize, randblock );
}

int main( int argc, char ** argv ) {
    simple_options::options opts{ "randcopy" };
    size_t bufsize, randblock;
    opts.basic_option( "help,h", "Print description and exit" )
        .basic_option( "bufsize,b", "Buffer size in frames",
                       simple_options::value<size_t>( &bufsize )->default_value( 1 ) )
        .basic_option( "randblock,r", "Size of random blocks (how many frames per RNG hit)",
                       simple_options::value<size_t>( &randblock )->default_value( 1 ) )
        .positional( "input", "Input file" )
        .positional( "output1", "First output file" )
        .positional( "output2", "Second output file" )
        .parse( argc, argv );

    using namespace std::placeholders;
    return checked_invoke( opts, std::array{ "input", "output1", "output2" },
                           std::bind( do_copy, _1, _2, _3, bufsize, randblock ) );
}

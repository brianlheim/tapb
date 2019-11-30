// pan iterator utilities
#pragma once

#include "breakpoint/breakpoint.hpp"
#include "util/pan_utils.hpp"

#include <cstdint>
#include <span>

// Represents a breakpoint envelope generator designed for multiplying with a stereo signal for
// panning.

class stereo_envelope_generator {
public:
    // Points: breakpoint envelope. Assumed to start from time 0. No assumption made about last
    // time. Sample rate: used for conversion from seconds to samples. Bufsize: max number of frames
    // that will be requested at one time.
    stereo_envelope_generator( const breakpoint::point_list & points,
                               const uint32_t sample_rate,
                               const size_t bufsize ):
        _buffer( bufsize * 2, audio_math::root_two_div_two() ),
        _points( make_points( points, sample_rate ) ),
        _current_point( _points.begin() ),
        _index( 0 ),
        // If there were no points to begin with, _buffer is already filled with zeros.
        _finalized_frame_count( _points.size() == 0 ? bufsize : 0 ) {
    }

    // Noncopyable since we keep a reference to the inside of the vector. Can still move though.
    stereo_envelope_generator( const stereo_envelope_generator & ) = delete;
    stereo_envelope_generator & operator=( const stereo_envelope_generator & ) = delete;

    // If `n` is greater than than bufsize, an empty span is returned and no other action is taken.
    // Otherwise, generate pan data for the next `n` frames. The returned span is of size n*2.
    std::span<float> next_frames( const size_t n ) noexcept {
        if ( n > bufsize() )
            return {};

        generate_frames( n );
        return std::span<float>( _buffer.data(), n * 2 );
    }

private:
    size_t bufsize() const noexcept {
        return _buffer.size() / 2;
    }

    struct sample_point {
        uint64_t time_sample;
        double value;
    };
    using Points = std::vector<sample_point>;

    Points make_points( const breakpoint::point_list & points,
                        const uint32_t sample_rate ) const noexcept {
        Points result;
        result.reserve( points.size() );
        for ( auto & point : points ) {
            result.push_back( { uint64_t( point.time_secs * sample_rate ), point.value } );
        }
        return result;
    }

    void generate_frames( const size_t n ) noexcept {
        // Reuse samples
        if ( n <= _finalized_frame_count )
            return;

        // We are certain at this point that n <= bufsize, and therefore that _points.size() > 0
        // N.B. this results in wasted work if we are past the end and then we receive requests
        // for 2, 4, 6, etc. frames. The previous frames will be recalculated. However this is
        // unlikely to happen in practice.
        if ( next( _current_point ) == end( _points ) )
            _finalized_frame_count = n;

        for ( size_t i = 0; i < n; ++i ) {
            auto val = val_at( _index + i );
            auto pan = constant_power_pan<float>( val );
            _buffer[i * 2] = pan.left;
            _buffer[i * 2 + 1] = pan.right;
        }

        _index += n;
    }

    float val_at( size_t i ) noexcept {
        if ( next( _current_point ) == end( _points ) ) {
            return float( _current_point->value );
        } else {
            if ( i == next( _current_point )->time_sample ) {
                _current_point++;
                if ( next( _current_point ) == end( _points ) ) {
                    return float( _current_point->value );
                }
            }

            uint64_t span = next( _current_point )->time_sample - _current_point->time_sample;
            float interp = float( i - _current_point->time_sample ) / float( span );
            return ( interp * ( next( _current_point )->value - _current_point->value ) )
                + _current_point->value;
        }
    }

    std::vector<float> _buffer; // .size() is used to hold bufsize
    Points _points;
    Points::const_iterator _current_point;
    size_t _index;
    // used to indicate whether any more samples need to be dynamically calculated
    size_t _finalized_frame_count;
};

#pragma once

#include "breakpoint/breakpoint.hpp"

#include <cstdint>
#include <span>

// A general purpose breakpoint envelope generator.

template <typename Derived, size_t Channels> class envelop_generator_base {
public:
    // Points: breakpoint envelope. Assumed to start from time 0. No assumption made about last
    // time. Sample rate: used for conversion from seconds to samples. Bufsize: max number of frames
    // that will be requested at one time.
    envelop_generator_base( const breakpoint::point_list & points,
                            const uint32_t sample_rate,
                            const size_t bufsize,
                            const float buffer_fill ):
        _buffer( bufsize * Channels, buffer_fill ),
        _points( make_points( points, sample_rate ) ),
        _current_point( _points.begin() ),
        _index( 0 ),
        // If there were no points to begin with, _buffer is already filled with zeros.
        _finalized_frame_count( _points.size() == 0 ? bufsize : 0 ) {
    }

    // Noncopyable since we keep a reference to the inside of the vector. Can still move though.
    envelop_generator_base( const envelop_generator_base & ) = delete;
    envelop_generator_base & operator=( const envelop_generator_base & ) = delete;

    // If `n` is greater than than bufsize, an empty span is returned and no other action is taken.
    // Otherwise, generate pan data for the next `n` frames. The returned span is of size n*2.
    std::span<float> next_frames( const size_t n ) noexcept {
        if ( n > bufsize() )
            return {};

        generate_frames( n );
        return std::span<float>( _buffer.data(), n * Channels );
    }

protected:
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

private:
    size_t bufsize() const noexcept {
        return _buffer.size() / Channels;
    }

    struct sample_point {
        uint64_t time_sample;
        double value;
    };
    using Points = std::vector<sample_point>;

    Points make_points( const breakpoint::point_list & points, const uint32_t sample_rate ) const
        noexcept {
        Points result;
        result.reserve( points.size() );
        for ( auto & point : points ) {
            result.push_back( { uint64_t( point.time_secs * sample_rate ), point.value } );
        }
        return result;
    }

    constexpr const Derived * derived() const noexcept {
        return static_cast<const Derived *>( this );
    }

    constexpr Derived * derived() noexcept {
        return static_cast<Derived *>( this );
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

        derived()->generate_frames( _buffer, _index, n );
        _index += n;
    }

    std::vector<float> _buffer; // .size() is used to hold bufsize
    Points _points;
    typename Points::const_iterator _current_point;
    size_t _index;
    // used to indicate whether any more samples need to be dynamically calculated
    size_t _finalized_frame_count;
};

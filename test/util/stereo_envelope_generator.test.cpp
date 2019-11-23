#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

#include "util/stereo_envelope_generator.hpp"

#include <ios>
#include <sstream>

using namespace breakpoint;
using namespace Catch;
constexpr float rtdt = audio_math::root_two_div_two();

class SpanAllEqualMatcher : public Catch::MatcherBase<std::span<float>> {
public:
    SpanAllEqualMatcher( float val ): _val( val ) {
    }

    bool match( const std::span<float> & span ) const override {
        return std::all_of( begin( span ), end( span ), [this]( float x ) { return x == _val; } );
    }

    virtual std::string describe() const override {
        std::ostringstream ss;
        ss << "has elements all equal to " << _val;
        return ss.str();
    }

private:
    float _val;
};

inline SpanAllEqualMatcher IsAllEqual( float val ) {
    return { val };
}

TEST_CASE( "Construction" ) {
    stereo_envelope_generator( {}, 0, 0 );
    stereo_envelope_generator( {}, 1, 1 );
    stereo_envelope_generator( {}, 100, 10 );
    stereo_envelope_generator( { { 0.0, 0.0 } }, 100, 10 );
}

TEST_CASE( "Empty envelope all equal power" ) {
    stereo_envelope_generator gen( {}, 1, 10 );
    auto span = gen.next_frames( 10 );
    CHECK( span.size() == 20 );
    CHECK_THAT( span, IsAllEqual( audio_math::root_two_div_two() ) );
}

TEST_CASE( "Bufsize too big empty span" ) {
    stereo_envelope_generator gen( {}, 1, 10 );
    auto span = gen.next_frames( 20 );
    CHECK( span.size() == 0 );
}

TEST_CASE( "Zero env all equal power" ) {
    stereo_envelope_generator gen( { { 0.0, 0.0 } }, 1, 10 );
    auto span = gen.next_frames( 3 );
    CHECK( span.size() == 6 );
    CHECK_THAT( span, IsAllEqual( audio_math::root_two_div_two() ) );
}

// Use a decent margin (1e-15)
#define CHECK_APPROX( _span_, ... )                                                                \
    auto span_vec = std::vector( _span_.data(), _span_.data() + _span_.size() );                   \
    auto other_vec = std::vector( __VA_ARGS__ );                                                   \
    CHECK_THAT( span_vec, Matchers::Approx( other_vec ).margin( 1e-15 ) )

TEST_CASE( "One env all panned right" ) {
    Catch::StringMaker<float>::precision = 18;
    stereo_envelope_generator gen( { { 0.0, 1.0 } }, 1, 10 );
    auto span = gen.next_frames( 3 );
    CHECK_APPROX( span, { 0.f, 1.f, 0.f, 1.f, 0.f, 1.f } );
}

TEST_CASE( "1 to 1 all 1s" ) {
    stereo_envelope_generator gen( { { 0.0, 1.0 }, { 1.0, 1.0 } }, 1, 10 );
    auto span = gen.next_frames( 3 );
    CHECK_APPROX( span, { 0.f, 1.f, 0.f, 1.f, 0.f, 1.f } );
}

TEST_CASE( "Basic jumps" ) {
    stereo_envelope_generator gen( { { 0.0, 0.0 }, { 1.0, 1.0 }, { 2.0, -1.0 } }, 1, 10 );
    auto span = gen.next_frames( 3 );
    CHECK_APPROX( span, { rtdt, rtdt, 0.f, 1.f, 1.f, 0.f } );
}

TEST_CASE( "Interpolation" ) {
    stereo_envelope_generator gen( { { 0.0, 0.0 }, { 1.0, 1.0 } }, 4, 10 );
    auto span = gen.next_frames( 5 );

    auto qtr = constant_power_pan<float>( 0.25 );
    auto half = constant_power_pan<float>( 0.5 );
    auto three_qtr = constant_power_pan<float>( 0.75 );
    CHECK_APPROX( span,
                  { rtdt, rtdt, qtr.left, qtr.right, half.left, half.right, three_qtr.left,
                    three_qtr.right, 0.f, 1.f } );
}

TEST_CASE( "Interpolation and back" ) {
    stereo_envelope_generator gen( { { 0.0, 0.0 }, { 1.0, 1.0 }, { 2.0, 0.0 } }, 4, 10 );
    auto span = gen.next_frames( 9 );

    auto qtr = constant_power_pan<float>( 0.25 );
    auto half = constant_power_pan<float>( 0.5 );
    auto three_qtr = constant_power_pan<float>( 0.75 );
    CHECK_APPROX( span,
                  { rtdt, rtdt, qtr.left, qtr.right, half.left, half.right, three_qtr.left,
                    three_qtr.right, 0.f, 1.f, three_qtr.left, three_qtr.right, half.left,
                    half.right, qtr.left, qtr.right, rtdt, rtdt } );
}

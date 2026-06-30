// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

#include <limits>

using namespace boost::safe_numbers;

// The numeric_limits specializations for the bounded types are otherwise only
// checked through constexpr/static_assert, which evaluates the accessor bodies
// at compile time. These tests call them in a runtime context so the function
// bodies are actually executed. epsilon(), round_error(), infinity(),
// quiet_NaN(), signaling_NaN() and denorm_min() all forward to min() by design.

void test_bounded_uint_limits()
{
    using T = bounded_uint<10u, 200u>;
    using lim = std::numeric_limits<T>;

    const auto lo {lim::min()};
    const auto hi {lim::max()};
    BOOST_TEST(lo == T{u8{10}});
    BOOST_TEST(hi == T{u8{200}});
    BOOST_TEST(lim::lowest() == lo);
    BOOST_TEST(lim::epsilon() == lo);
    BOOST_TEST(lim::round_error() == lo);
    BOOST_TEST(lim::infinity() == lo);
    BOOST_TEST(lim::quiet_NaN() == lo);
    BOOST_TEST(lim::signaling_NaN() == lo);
    BOOST_TEST(lim::denorm_min() == lo);
}

void test_bounded_int_limits()
{
    using T = bounded_int<-100, 100>;
    using lim = std::numeric_limits<T>;

    const auto lo {lim::min()};
    const auto hi {lim::max()};
    BOOST_TEST(lo == T{i8{-100}});
    BOOST_TEST(hi == T{i8{100}});
    BOOST_TEST(lim::lowest() == lo);
    BOOST_TEST(lim::epsilon() == lo);
    BOOST_TEST(lim::round_error() == lo);
    BOOST_TEST(lim::infinity() == lo);
    BOOST_TEST(lim::quiet_NaN() == lo);
    BOOST_TEST(lim::signaling_NaN() == lo);
    BOOST_TEST(lim::denorm_min() == lo);
}

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

void test_bounded_float_limits()
{
    using T = bounded_float<-1.0f, 1.0f>;
    using lim = std::numeric_limits<T>;

    const auto lo {lim::min()};
    const auto hi {lim::max()};
    BOOST_TEST(lo == T{f32{-1.0f}});
    BOOST_TEST(hi == T{f32{1.0f}});
    BOOST_TEST(lim::lowest() == lo);
    BOOST_TEST(lim::epsilon() == lo);
    BOOST_TEST(lim::round_error() == lo);
    BOOST_TEST(lim::infinity() == lo);
    BOOST_TEST(lim::quiet_NaN() == lo);
    BOOST_TEST(lim::signaling_NaN() == lo);
    BOOST_TEST(lim::denorm_min() == lo);
}

#endif

int main()
{
    test_bounded_uint_limits();
    test_bounded_int_limits();

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT
    test_bounded_float_limits();
#endif

    return boost::report_errors();
}

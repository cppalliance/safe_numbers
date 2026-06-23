// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/floats.hpp>
#include <boost/safe_numbers/charconv.hpp>
#include <random>
#include <limits>

#endif

using namespace boost::safe_numbers;
using boost::charconv::to_chars;
using boost::charconv::from_chars;
using boost::charconv::chars_format;

static std::mt19937_64 rng {42};

// general and scientific produce a round-trippable shortest representation, so the
// recovered value must compare exactly equal to the original. (hex is intentionally
// excluded here: boost::charconv's own hex to_chars/from_chars do not round-trip a
// raw double, which is a charconv-level concern, not a property of this wrapper.)
template <typename T>
void roundtrip(const T value, const chars_format fmt)
{
    char buffer[256];
    const auto r_to {to_chars(buffer, buffer + sizeof(buffer), value, fmt)};
    BOOST_TEST(r_to.ec == std::errc{});

    T back {};
    const auto r_from {from_chars(buffer, r_to.ptr, back, fmt)};
    BOOST_TEST(r_from.ec == std::errc{});
    BOOST_TEST(back == value);
}

template <typename T>
void test_roundtrip()
{
    using basis_type = typename T::basis_type;

    for (const auto fmt : {chars_format::general, chars_format::scientific})
    {
        // Representative finite values
        for (const auto v : {static_cast<basis_type>(0.0), static_cast<basis_type>(1.0),
                             static_cast<basis_type>(-1.0), static_cast<basis_type>(3.141592653589793),
                             static_cast<basis_type>(-2.5e10), static_cast<basis_type>(1.25e-7)})
        {
            roundtrip(T{v}, fmt);
        }

        roundtrip(T{std::numeric_limits<basis_type>::max()}, fmt);
        roundtrip(T{std::numeric_limits<basis_type>::lowest()}, fmt);
        roundtrip(T{std::numeric_limits<basis_type>::min()}, fmt);

        // Random finite values
        std::uniform_real_distribution<basis_type> dist {static_cast<basis_type>(-1.0e6),
                                                         static_cast<basis_type>(1.0e6)};
        for (int i {}; i < 512; ++i)
        {
            roundtrip(T{dist(rng)}, fmt);
        }
    }
}

// The default overloads (general format) must be usable without naming a format.
template <typename T>
void test_default_format()
{
    using basis_type = typename T::basis_type;

    char buffer[256];
    const T value {static_cast<basis_type>(42.5)};

    const auto r_to {to_chars(buffer, buffer + sizeof(buffer), value)};
    BOOST_TEST(r_to.ec == std::errc{});

    T back {};
    const auto r_from {from_chars(buffer, r_to.ptr, back)};
    BOOST_TEST(r_from.ec == std::errc{});
    BOOST_TEST(back == value);
}

// The (format, precision) overload must compile and write without error.
template <typename T>
void test_precision()
{
    using basis_type = typename T::basis_type;

    char buffer[256];
    const T value {static_cast<basis_type>(3.141592653589793)};

    const auto r_to {to_chars(buffer, buffer + sizeof(buffer), value, chars_format::fixed, 4)};
    BOOST_TEST(r_to.ec == std::errc{});
}

// The hex format path of the wrapper must at least emit without error.
template <typename T>
void test_hex_emit()
{
    using basis_type = typename T::basis_type;

    char buffer[256];
    const auto r {to_chars(buffer, buffer + sizeof(buffer), T{static_cast<basis_type>(1.5)}, chars_format::hex)};
    BOOST_TEST(r.ec == std::errc{});
    BOOST_TEST(r.ptr != buffer);
}

// Infinity round-trips and compares equal (NAN would not, by definition).
template <typename T>
void test_infinity()
{
    using basis_type = typename T::basis_type;

    const T pos_inf {std::numeric_limits<basis_type>::infinity()};
    roundtrip(pos_inf, chars_format::general);

    const T neg_inf {-std::numeric_limits<basis_type>::infinity()};
    roundtrip(neg_inf, chars_format::general);
}

int main()
{
    test_roundtrip<f32>();
    test_roundtrip<f64>();

    test_default_format<f32>();
    test_default_format<f64>();

    test_precision<f32>();
    test_precision<f64>();

    test_hex_emit<f32>();
    test_hex_emit<f64>();

    test_infinity<f32>();
    test_infinity<f64>();

    return boost::report_errors();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/core/lightweight_test.hpp>

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/bounded_floats.hpp>
#include <boost/safe_numbers/floats.hpp>

#endif

#include <bit>
#include <cstdint>
#include <stdexcept>

using namespace boost::safe_numbers;

// Bit-pattern equality avoids -Wfloat-equal in BOOST_TEST_EQ on raw floats.

void test_conversion_to_underlying_float()
{
    const bounded_float<-1.0f, 1.0f> a {f32{0.5f}};
    const auto raw {static_cast<float>(a)};
    BOOST_TEST_EQ(std::bit_cast<std::uint32_t>(raw), std::bit_cast<std::uint32_t>(0.5f));
}

void test_conversion_to_underlying_double()
{
    const bounded_float<-1.0e100, 1.0e100> a {f64{1.0e50}};
    const auto raw {static_cast<double>(a)};
    BOOST_TEST_EQ(std::bit_cast<std::uint64_t>(raw), std::bit_cast<std::uint64_t>(1.0e50));
}

void test_widening_conversion()
{
    const bounded_float<-1.0f, 1.0f> a {f32{0.5f}};
    const auto widened {static_cast<double>(a)};
    BOOST_TEST_EQ(std::bit_cast<std::uint64_t>(widened), std::bit_cast<std::uint64_t>(0.5));
}

void test_narrowing_conversion_in_range()
{
    const bounded_float<-1.0e10, 1.0e10> a {f64{1.0e5}};
    const auto narrowed {static_cast<float>(a)};
    BOOST_TEST_EQ(std::bit_cast<std::uint32_t>(narrowed), std::bit_cast<std::uint32_t>(1.0e5f));
}

void test_narrowing_conversion_overflow()
{
    // 1e40 is within double's range but greater than FLT_MAX (~3.4e38)
    const bounded_float<-1.0e100, 1.0e100> a {f64{1.0e40}};
    BOOST_TEST_THROWS((void)static_cast<float>(a), std::overflow_error);
}

void test_to_basis()
{
    const bounded_float<-1.0f, 1.0f> a {f32{0.5f}};
    const f32 b {a.to_basis()};
    const f32 expected {0.5f};
    BOOST_TEST(b == expected);
}

void test_conversion_between_bounded_floats()
{
    const bounded_float<-1.0f, 1.0f> a {f32{0.5f}};
    const auto b {static_cast<bounded_float<-2.0f, 2.0f>>(a)};
    const bounded_float<-2.0f, 2.0f> expected {f32{0.5f}};
    BOOST_TEST(b == expected);

    // Conversion that puts value out of new bounds throws
    const bounded_float<-1.0f, 1.0f> c {f32{0.75f}};
    using narrow_t = bounded_float<-0.5f, 0.5f>;
    BOOST_TEST_THROWS((void)static_cast<narrow_t>(c), std::domain_error);
}

int main()
{
    test_conversion_to_underlying_float();
    test_conversion_to_underlying_double();
    test_widening_conversion();
    test_narrowing_conversion_in_range();
    test_narrowing_conversion_overflow();
    test_to_basis();
    test_conversion_between_bounded_floats();

    return boost::report_errors();
}

#else // BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

int main() { return 0; }

#endif

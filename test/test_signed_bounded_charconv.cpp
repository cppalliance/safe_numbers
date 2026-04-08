// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wsign-compare"
#  pragma clang diagnostic ignored "-Woverflow"

#  if (__clang_major__ >= 10 && !defined(__APPLE__)) || __clang_major__ >= 13
#    pragma clang diagnostic ignored "-Wdeprecated-copy"
#  endif

#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wundef"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Woverflow"

#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4389)
#  pragma warning(disable : 4127)
#  pragma warning(disable : 4305)
#  pragma warning(disable : 4309)
#endif

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_COMPARE
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION

#include <boost/random/uniform_int_distribution.hpp>

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

// Ignore [[nodiscard]] on the test that we know are going to throw
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-result"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(_MSC_VER)
#  pragma warning (push)
#  pragma warning (disable: 4834)
#endif

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <random>
#include <cstring>
#include <limits>
#include <type_traits>
#include <stdexcept>

#endif

using namespace boost::safe_numbers;
using boost::charconv::to_chars;
using boost::charconv::from_chars;

static std::mt19937_64 rng {42};

// bounded_int has no default constructor, so use a copy of value as placeholder
template <typename T>
void test(T value, int base = 10)
{
    char buffer[256];
    const auto r_to {to_chars(buffer, buffer + sizeof(buffer), value, base)};
    BOOST_TEST(r_to.ec == std::errc{});

    auto return_val {value};
    const auto r_from {from_chars(buffer, r_to.ptr, return_val, base)};
    BOOST_TEST(r_from.ec == std::errc{});

    BOOST_TEST_EQ(value, return_val);
}

// -----------------------------------------------
// Roundtrip tests for i8-backed bounded_int
// -----------------------------------------------

void test_i8_range_roundtrip()
{
    using type = bounded_int<-100, 100>;

    // Boundary values
    test(type{i8{-100}});
    test(type{i8{100}});

    // Negative mid-range
    test(type{i8{-42}});
    test(type{i8{-1}});

    // Zero and positive mid-range
    test(type{i8{0}});
    test(type{i8{1}});
    test(type{i8{50}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::int8_t> dist {-100, 100};
    for (int i {}; i < 256; ++i)
    {
        test(type{i8{dist(rng)}});
    }
}

void test_i8_negative_only_roundtrip()
{
    using type = bounded_int<-120, -10>;

    // Boundary values
    test(type{i8{-120}});
    test(type{i8{-10}});

    // Mid-range
    test(type{i8{-50}});
    test(type{i8{-75}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::int8_t> dist {-120, -10};
    for (int i {}; i < 256; ++i)
    {
        test(type{i8{dist(rng)}});
    }
}

// -----------------------------------------------
// Roundtrip tests for i16-backed bounded_int
// -----------------------------------------------

void test_i16_range_roundtrip()
{
    using type = bounded_int<-1000, 1000>;

    // Boundary values
    test(type{i16{-1000}});
    test(type{i16{1000}});

    // Negative mid-range
    test(type{i16{-42}});
    test(type{i16{-1}});

    // Zero and positive mid-range
    test(type{i16{0}});
    test(type{i16{500}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::int16_t> dist {-1000, 1000};
    for (int i {}; i < 1024; ++i)
    {
        test(type{i16{dist(rng)}});
    }
}

void test_i16_negative_min_roundtrip()
{
    using type = bounded_int<-30000, 30000>;

    // Boundary values
    test(type{i16{-30000}});
    test(type{i16{30000}});

    // Mid-range
    test(type{i16{-15000}});
    test(type{i16{0}});
    test(type{i16{15000}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::int16_t> dist {-30000, 30000};
    for (int i {}; i < 1024; ++i)
    {
        test(type{i16{dist(rng)}});
    }
}

// -----------------------------------------------
// Roundtrip tests for i32-backed bounded_int
// -----------------------------------------------

void test_i32_range_roundtrip()
{
    using type = bounded_int<-100000, 100000>;

    // Boundary values
    test(type{i32{-100000}});
    test(type{i32{100000}});

    // Negative mid-range
    test(type{i32{-42}});
    test(type{i32{-1}});

    // Zero and positive mid-range
    test(type{i32{0}});
    test(type{i32{50000}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::int32_t> dist {-100000, 100000};
    for (int i {}; i < 1024; ++i)
    {
        test(type{i32{dist(rng)}});
    }
}

void test_i32_wide_range_roundtrip()
{
    using type = bounded_int<-2000000000, 2000000000>;

    // Boundary values
    test(type{i32{-2000000000}});
    test(type{i32{2000000000}});

    // Mid-range
    test(type{i32{-1000000000}});
    test(type{i32{0}});
    test(type{i32{1000000000}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::int32_t> dist {-2000000000, 2000000000};
    for (int i {}; i < 1024; ++i)
    {
        test(type{i32{dist(rng)}});
    }
}

// -----------------------------------------------
// Roundtrip tests for i64-backed bounded_int
// -----------------------------------------------

void test_i64_range_roundtrip()
{
    using type = bounded_int<-3000000000LL, 3000000000LL>;

    // Boundary values
    test(type{i64{-3000000000LL}});
    test(type{i64{3000000000LL}});

    // Negative mid-range
    test(type{i64{-42}});
    test(type{i64{-1}});

    // Zero and positive mid-range
    test(type{i64{0}});
    test(type{i64{1500000000LL}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::int64_t> dist {-3000000000LL, 3000000000LL};
    for (int i {}; i < 1024; ++i)
    {
        test(type{i64{dist(rng)}});
    }
}

void test_i64_wide_range_roundtrip()
{
    using type = bounded_int<-4000000000000LL, 4000000000000LL>;

    // Boundary values
    test(type{i64{-4000000000000LL}});
    test(type{i64{4000000000000LL}});

    // Mid-range
    test(type{i64{-2000000000000LL}});
    test(type{i64{0}});
    test(type{i64{2000000000000LL}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::int64_t> dist {-4000000000000LL, 4000000000000LL};
    for (int i {}; i < 1024; ++i)
    {
        test(type{i64{dist(rng)}});
    }
}

// -----------------------------------------------
// Multi-base roundtrip tests
// -----------------------------------------------

void test_multiple_bases()
{
    using type8 = bounded_int<-100, 100>;

    for (int base {2}; base <= 36; ++base)
    {
        test(type8{i8{-100}}, base);
        test(type8{i8{-42}}, base);
        test(type8{i8{0}}, base);
        test(type8{i8{42}}, base);
        test(type8{i8{100}}, base);
    }

    using type16 = bounded_int<-1000, 1000>;

    for (int base {2}; base <= 36; ++base)
    {
        test(type16{i16{-1000}}, base);
        test(type16{i16{0}}, base);
        test(type16{i16{1000}}, base);
    }

    using type32 = bounded_int<-100000, 100000>;

    for (int base {2}; base <= 36; ++base)
    {
        test(type32{i32{-100000}}, base);
        test(type32{i32{0}}, base);
        test(type32{i32{100000}}, base);
    }

    using type64 = bounded_int<-3000000000LL, 3000000000LL>;

    for (int base {2}; base <= 36; ++base)
    {
        test(type64{i64{-3000000000LL}}, base);
        test(type64{i64{0}}, base);
        test(type64{i64{3000000000LL}}, base);
    }
}

// -----------------------------------------------
// from_chars with out-of-bounds values
// -----------------------------------------------

void test_from_chars_out_of_bounds()
{
    // Value above maximum but within basis type range should throw
    {
        using type = bounded_int<-100, 100>;
        const char str[] = "120";
        auto val {type{i8{50}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // Value below minimum but within basis type range should throw
    {
        using type = bounded_int<-100, 100>;
        const char str[] = "-120";
        auto val {type{i8{50}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // Value above max for asymmetric range
    {
        using type = bounded_int<-50, 50>;
        const char str[] = "51";
        auto val {type{i8{0}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // Value below min for asymmetric range
    {
        using type = bounded_int<-50, 50>;
        const char str[] = "-51";
        auto val {type{i8{0}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // i16 range: value above max
    {
        using type = bounded_int<-1000, 1000>;
        const char str[] = "1001";
        auto val {type{i16{500}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // i16 range: value below min
    {
        using type = bounded_int<-1000, 1000>;
        const char str[] = "-1001";
        auto val {type{i16{500}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // i32 range: value above max
    {
        using type = bounded_int<-100000, 100000>;
        const char str[] = "100001";
        auto val {type{i32{50000}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // i32 range: value below min
    {
        using type = bounded_int<-100000, 100000>;
        const char str[] = "-100001";
        auto val {type{i32{50000}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }
}

int main()
{
    test_i8_range_roundtrip();
    test_i8_negative_only_roundtrip();
    test_i16_range_roundtrip();
    test_i16_negative_min_roundtrip();
    test_i32_range_roundtrip();
    test_i32_wide_range_roundtrip();
    test_i64_range_roundtrip();
    test_i64_wide_range_roundtrip();
    test_multiple_bases();
    test_from_chars_out_of_bounds();

    return boost::report_errors();
}

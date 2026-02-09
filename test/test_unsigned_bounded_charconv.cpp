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

static std::mt19937_64 rng {42};

// bounded_uint has no default constructor, so use a copy of value as placeholder
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
// Roundtrip tests for u8-backed bounded_uint
// -----------------------------------------------

void test_u8_range_roundtrip()
{
    using type = bounded_uint<0u, 200u>;

    // Boundary values
    test(type{u8{0}});
    test(type{u8{200}});

    // Mid-range
    test(type{u8{1}});
    test(type{u8{100}});
    test(type{u8{150}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::uint8_t> dist {0, 200};
    for (int i {}; i < 256; ++i)
    {
        test(type{u8{dist(rng)}});
    }
}

void test_u8_nonzero_min_roundtrip()
{
    using type = bounded_uint<10u, 100u>;

    // Boundary values
    test(type{u8{10}});
    test(type{u8{100}});

    // Mid-range
    test(type{u8{50}});
    test(type{u8{75}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::uint8_t> dist {10, 100};
    for (int i {}; i < 256; ++i)
    {
        test(type{u8{dist(rng)}});
    }
}

// -----------------------------------------------
// Roundtrip tests for u16-backed bounded_uint
// -----------------------------------------------

void test_u16_range_roundtrip()
{
    using type = bounded_uint<0u, 40000u>;

    // Boundary values
    test(type{u16{0}});
    test(type{u16{40000}});

    // Mid-range
    test(type{u16{1}});
    test(type{u16{20000}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::uint16_t> dist {0, 40000};
    for (int i {}; i < 1024; ++i)
    {
        test(type{u16{dist(rng)}});
    }
}

void test_u16_nonzero_min_roundtrip()
{
    using type = bounded_uint<256u, 65535u>;

    // Boundary values
    test(type{u16{256}});
    test(type{u16{65535}});

    // Mid-range
    test(type{u16{1000}});
    test(type{u16{30000}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::uint16_t> dist {256, 65535};
    for (int i {}; i < 1024; ++i)
    {
        test(type{u16{dist(rng)}});
    }
}

// -----------------------------------------------
// Roundtrip tests for u32-backed bounded_uint
// -----------------------------------------------

void test_u32_range_roundtrip()
{
    using type = bounded_uint<0u, 100000u>;

    // Boundary values
    test(type{u32{0}});
    test(type{u32{100000}});

    // Mid-range
    test(type{u32{1}});
    test(type{u32{50000}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::uint32_t> dist {0, 100000};
    for (int i {}; i < 1024; ++i)
    {
        test(type{u32{dist(rng)}});
    }
}

void test_u32_nonzero_min_roundtrip()
{
    using type = bounded_uint<65536u, 4294967295u>;

    // Boundary values
    test(type{u32{65536}});
    test(type{u32{4294967295u}});

    // Mid-range
    test(type{u32{1000000}});
    test(type{u32{2000000000u}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::uint32_t> dist {65536, 4294967295u};
    for (int i {}; i < 1024; ++i)
    {
        test(type{u32{dist(rng)}});
    }
}

// -----------------------------------------------
// Roundtrip tests for u64-backed bounded_uint
// -----------------------------------------------

void test_u64_range_roundtrip()
{
    using type = bounded_uint<0ULL, 5'000'000'000ULL>;

    // Boundary values
    test(type{u64{0}});
    test(type{u64{5'000'000'000ULL}});

    // Mid-range
    test(type{u64{1}});
    test(type{u64{2'500'000'000ULL}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::uint64_t> dist {0, 5'000'000'000ULL};
    for (int i {}; i < 1024; ++i)
    {
        test(type{u64{dist(rng)}});
    }
}

void test_u64_nonzero_min_roundtrip()
{
    using type = bounded_uint<4294967296ULL, UINT64_MAX>;

    // Boundary values
    test(type{u64{4294967296ULL}});
    test(type{u64{UINT64_MAX}});

    // Mid-range
    test(type{u64{5'000'000'000ULL}});
    test(type{u64{UINT64_MAX / 2}});

    // Random values within bounds
    boost::random::uniform_int_distribution<std::uint64_t> dist {4294967296ULL, UINT64_MAX};
    for (int i {}; i < 1024; ++i)
    {
        test(type{u64{dist(rng)}});
    }
}

// -----------------------------------------------
// Multi-base roundtrip tests
// -----------------------------------------------

void test_multiple_bases()
{
    using type = bounded_uint<0u, 200u>;

    for (int base {2}; base <= 36; ++base)
    {
        test(type{u8{0}}, base);
        test(type{u8{1}}, base);
        test(type{u8{100}}, base);
        test(type{u8{200}}, base);
    }

    using type16 = bounded_uint<0u, 40000u>;

    for (int base {2}; base <= 36; ++base)
    {
        test(type16{u16{0}}, base);
        test(type16{u16{1000}}, base);
        test(type16{u16{40000}}, base);
    }

    using type32 = bounded_uint<0u, 100000u>;

    for (int base {2}; base <= 36; ++base)
    {
        test(type32{u32{0}}, base);
        test(type32{u32{50000}}, base);
        test(type32{u32{100000}}, base);
    }

    using type64 = bounded_uint<0ULL, 5'000'000'000ULL>;

    for (int base {2}; base <= 36; ++base)
    {
        test(type64{u64{0}}, base);
        test(type64{u64{2'500'000'000ULL}}, base);
        test(type64{u64{5'000'000'000ULL}}, base);
    }
}

// -----------------------------------------------
// from_chars with out-of-bounds values
// -----------------------------------------------

void test_from_chars_out_of_bounds()
{
    // Value above maximum should throw during from_chars
    {
        using type = bounded_uint<0u, 100u>;
        const char str[] = "200";
        auto val {type{u8{50}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // Value below minimum should throw during from_chars
    {
        using type = bounded_uint<10u, 200u>;
        const char str[] = "5";
        auto val {type{u8{50}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // Value of zero below non-zero minimum
    {
        using type = bounded_uint<10u, 200u>;
        const char str[] = "0";
        auto val {type{u8{50}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // u16 range: value above max
    {
        using type = bounded_uint<0u, 1000u>;
        const char str[] = "1001";
        auto val {type{u16{500}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // u16 range: value below min
    {
        using type = bounded_uint<500u, 1000u>;
        const char str[] = "499";
        auto val {type{u16{500}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }

    // u32 range: value above max
    {
        using type = bounded_uint<0u, 100000u>;
        const char str[] = "100001";
        auto val {type{u32{50000}}};
        BOOST_TEST_THROWS(from_chars(str, str + std::strlen(str), val), std::domain_error);
    }
}

int main()
{
    test_u8_range_roundtrip();
    test_u8_nonzero_min_roundtrip();
    test_u16_range_roundtrip();
    test_u16_nonzero_min_roundtrip();
    test_u32_range_roundtrip();
    test_u32_nonzero_min_roundtrip();
    test_u64_range_roundtrip();
    test_u64_nonzero_min_roundtrip();
    test_multiple_bases();
    test_from_chars_out_of_bounds();

    return boost::report_errors();
}

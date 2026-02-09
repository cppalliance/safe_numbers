// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wconversion"
#endif

#define FMT_HEADER_ONLY

#if __has_include(<fmt/format.h>)

#include <boost/core/lightweight_test.hpp>
#include <boost/safe_numbers.hpp>
#include <boost/safe_numbers/fmt_format.hpp>
#include <fmt/format.h>

using namespace boost::safe_numbers;

template <typename T>
void test()
{
    const T x {42};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "42");
    BOOST_TEST_CSTR_EQ(fmt::format("{:08x}", x).c_str(), "0000002a");
    BOOST_TEST_CSTR_EQ(fmt::format("{:#010b}", x).c_str(), "0b00101010");
}

void test_boundary_values()
{
    // u8-backed
    {
        using type = bounded_uint<0u, 255u>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{0}).c_str(), "0");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{255}).c_str(), "255");
    }

    // u16-backed
    {
        using type = bounded_uint<0u, 40000u>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{0}).c_str(), "0");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{40000}).c_str(), "40000");
    }

    // u32-backed
    {
        using type = bounded_uint<0u, 100000u>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{0}).c_str(), "0");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{100000}).c_str(), "100000");
    }

    // u64-backed
    {
        using type = bounded_uint<0ULL, 5'000'000'000ULL>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{0}).c_str(), "0");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{5'000'000'000ULL}).c_str(), "5000000000");
    }

    // Non-zero minimum
    {
        using type = bounded_uint<10u, 200u>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{10}).c_str(), "10");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{100}).c_str(), "100");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{200}).c_str(), "200");
        BOOST_TEST_CSTR_EQ(fmt::format("{:04x}", type{200}).c_str(), "00c8");
    }
}

int main()
{
    test<bounded_uint<0u, 255u>>();
    test<bounded_uint<0u, 65535u>>();
    test<bounded_uint<0u, 4294967295u>>();
    test<bounded_uint<0ULL, UINT64_MAX>>();

    test_boundary_values();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif

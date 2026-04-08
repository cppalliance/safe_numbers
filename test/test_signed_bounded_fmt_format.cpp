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

template <typename T>
void test_negative()
{
    const T x {-42};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "-42");
}

void test_boundary_values()
{
    // i8-backed
    {
        using type = bounded_int<-100, 100>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{-100}).c_str(), "-100");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{0}).c_str(), "0");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{100}).c_str(), "100");
    }

    // i16-backed
    {
        using type = bounded_int<-1000, 1000>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{-1000}).c_str(), "-1000");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{0}).c_str(), "0");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{1000}).c_str(), "1000");
    }

    // i32-backed
    {
        using type = bounded_int<-100000, 100000>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{-100000}).c_str(), "-100000");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{0}).c_str(), "0");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{100000}).c_str(), "100000");
    }

    // i64-backed
    {
        using type = bounded_int<-3000000000LL, 3000000000LL>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{-3000000000LL}).c_str(), "-3000000000");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{0}).c_str(), "0");
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{3000000000LL}).c_str(), "3000000000");
    }

    // Negative minimum format
    {
        using type = bounded_int<-100, 100>;
        BOOST_TEST_CSTR_EQ(fmt::format("{}", type{-42}).c_str(), "-42");
        BOOST_TEST_CSTR_EQ(fmt::format("{:04x}", type{42}).c_str(), "002a");
    }
}

int main()
{
    using type8 = bounded_int<-100, 100>;
    using type16 = bounded_int<-1000, 1000>;
    using type32 = bounded_int<-100000, 100000>;
    using type64 = bounded_int<-3000000000LL, 3000000000LL>;

    test<type8>();
    test<type16>();
    test<type32>();
    test<type64>();

    test_negative<type8>();
    test_negative<type16>();
    test_negative<type32>();
    test_negative<type64>();

    test_boundary_values();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif

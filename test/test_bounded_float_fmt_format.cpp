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

void test_basic()
{
    const bounded_float<-100.0f, 100.0f> x {f32{42.5f}};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "42.5");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2f}", x).c_str(), "42.50");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2e}", x).c_str(), "4.25e+01");
    BOOST_TEST_CSTR_EQ(fmt::format("{:+.1f}", x).c_str(), "+42.5");
    BOOST_TEST_CSTR_EQ(fmt::format("{:>10.2f}", x).c_str(), "     42.50");
}

void test_negative()
{
    const bounded_float<-100.0, 100.0> x {f64{-42.5}};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "-42.5");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2f}", x).c_str(), "-42.50");
    BOOST_TEST_CSTR_EQ(fmt::format("{:+.1f}", x).c_str(), "-42.5");
}

void test_zero()
{
    const bounded_float<-1.0f, 1.0f> x {f32{0.0f}};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "0");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2f}", x).c_str(), "0.00");
}

int main()
{
    test_basic();
    test_negative();
    test_zero();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif

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
constexpr T make_value(double v) noexcept
{
    return T{static_cast<typename T::basis_type>(v)};
}

template <typename T>
void test()
{
    const T x {make_value<T>(42.5)};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "42.5");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2f}", x).c_str(), "42.50");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2e}", x).c_str(), "4.25e+01");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2E}", x).c_str(), "4.25E+01");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.3g}", x).c_str(), "42.5");
    BOOST_TEST_CSTR_EQ(fmt::format("{:+.1f}", x).c_str(), "+42.5");
    BOOST_TEST_CSTR_EQ(fmt::format("{:>10.2f}", x).c_str(), "     42.50");
    BOOST_TEST_CSTR_EQ(fmt::format("{:<10.2f}", x).c_str(), "42.50     ");
    BOOST_TEST_CSTR_EQ(fmt::format("{:^10.2f}", x).c_str(), "  42.50   ");
    BOOST_TEST_CSTR_EQ(fmt::format("{:08.2f}", x).c_str(), "00042.50");
}

template <typename T>
void test_negative()
{
    const T x {make_value<T>(-42.5)};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "-42.5");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2f}", x).c_str(), "-42.50");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2e}", x).c_str(), "-4.25e+01");
    BOOST_TEST_CSTR_EQ(fmt::format("{:+.1f}", x).c_str(), "-42.5");
}

template <typename T>
void test_zero()
{
    const T x {make_value<T>(0.0)};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "0");
    BOOST_TEST_CSTR_EQ(fmt::format("{:.2f}", x).c_str(), "0.00");
    BOOST_TEST_CSTR_EQ(fmt::format("{:+.2f}", x).c_str(), "+0.00");
}

int main()
{
    test<f32>();
    test<f64>();

    test_negative<f32>();
    test_negative<f64>();

    test_zero<f32>();
    test_zero<f64>();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif

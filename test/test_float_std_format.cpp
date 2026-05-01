// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_FORMAT

#include <format>

#endif

#endif

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_HAS_FORMAT

#include <boost/core/lightweight_test.hpp>

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

    BOOST_TEST_CSTR_EQ(std::format("{}", x).c_str(), "42.5");
    BOOST_TEST_CSTR_EQ(std::format("{:.2f}", x).c_str(), "42.50");
    BOOST_TEST_CSTR_EQ(std::format("{:.2e}", x).c_str(), "4.25e+01");
    BOOST_TEST_CSTR_EQ(std::format("{:.2E}", x).c_str(), "4.25E+01");
    BOOST_TEST_CSTR_EQ(std::format("{:.3g}", x).c_str(), "42.5");
    BOOST_TEST_CSTR_EQ(std::format("{:+.1f}", x).c_str(), "+42.5");
    BOOST_TEST_CSTR_EQ(std::format("{:>10.2f}", x).c_str(), "     42.50");
    BOOST_TEST_CSTR_EQ(std::format("{:<10.2f}", x).c_str(), "42.50     ");
    BOOST_TEST_CSTR_EQ(std::format("{:^10.2f}", x).c_str(), "  42.50   ");
    BOOST_TEST_CSTR_EQ(std::format("{:08.2f}", x).c_str(), "00042.50");
}

template <typename T>
void test_negative()
{
    const T x {make_value<T>(-42.5)};

    BOOST_TEST_CSTR_EQ(std::format("{}", x).c_str(), "-42.5");
    BOOST_TEST_CSTR_EQ(std::format("{:.2f}", x).c_str(), "-42.50");
    BOOST_TEST_CSTR_EQ(std::format("{:.2e}", x).c_str(), "-4.25e+01");
    BOOST_TEST_CSTR_EQ(std::format("{:+.1f}", x).c_str(), "-42.5");
}

template <typename T>
void test_zero()
{
    const T x {make_value<T>(0.0)};

    BOOST_TEST_CSTR_EQ(std::format("{}", x).c_str(), "0");
    BOOST_TEST_CSTR_EQ(std::format("{:.2f}", x).c_str(), "0.00");
    BOOST_TEST_CSTR_EQ(std::format("{:+.2f}", x).c_str(), "+0.00");
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

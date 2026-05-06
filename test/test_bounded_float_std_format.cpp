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

void test_basic()
{
    const bounded_float<-100.0f, 100.0f> x {f32{42.5f}};

    BOOST_TEST_CSTR_EQ(std::format("{}", x).c_str(), "42.5");
    BOOST_TEST_CSTR_EQ(std::format("{:.2f}", x).c_str(), "42.50");
    BOOST_TEST_CSTR_EQ(std::format("{:.2e}", x).c_str(), "4.25e+01");
    BOOST_TEST_CSTR_EQ(std::format("{:+.1f}", x).c_str(), "+42.5");
    BOOST_TEST_CSTR_EQ(std::format("{:>10.2f}", x).c_str(), "     42.50");
}

void test_negative()
{
    const bounded_float<-100.0, 100.0> x {f64{-42.5}};

    BOOST_TEST_CSTR_EQ(std::format("{}", x).c_str(), "-42.5");
    BOOST_TEST_CSTR_EQ(std::format("{:.2f}", x).c_str(), "-42.50");
    BOOST_TEST_CSTR_EQ(std::format("{:+.1f}", x).c_str(), "-42.5");
}

void test_zero()
{
    const bounded_float<-1.0f, 1.0f> x {f32{0.0f}};

    BOOST_TEST_CSTR_EQ(std::format("{}", x).c_str(), "0");
    BOOST_TEST_CSTR_EQ(std::format("{:.2f}", x).c_str(), "0.00");
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

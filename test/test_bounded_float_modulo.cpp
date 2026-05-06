// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/bounded_floats.hpp>
#include <boost/safe_numbers/floats.hpp>

#endif

#include <stdexcept>

using namespace boost::safe_numbers;

void test_modulo_in_bounds()
{
    const bounded_float<-100.0f, 100.0f> a {f32{7.5f}};
    const bounded_float<-100.0f, 100.0f> b {f32{3.0f}};
    const auto r {a % b};
    BOOST_TEST_EQ(static_cast<float>(r), 1.5f);
}

void test_modulo_by_zero()
{
    const bounded_float<-100.0f, 100.0f> a {f32{7.5f}};
    const bounded_float<-100.0f, 100.0f> b {f32{0.0f}};
    BOOST_TEST_THROWS((void)(a % b), std::domain_error);
}

void test_modulo_post_op_out_of_range()
{
    const bounded_float<5.0f, 10.0f> a {f32{7.5f}};
    const bounded_float<5.0f, 10.0f> b {f32{6.0f}};
    BOOST_TEST_THROWS((void)(a % b), std::domain_error);
}

void test_modulo_compound_assignment()
{
    bounded_float<-100.0f, 100.0f> a {f32{7.5f}};
    const bounded_float<-100.0f, 100.0f> b {f32{3.0f}};
    a %= b;
    BOOST_TEST_EQ(static_cast<float>(a), 1.5f);
}

int main()
{
    test_modulo_in_bounds();
    test_modulo_by_zero();
    test_modulo_post_op_out_of_range();
    test_modulo_compound_assignment();

    return boost::report_errors();
}

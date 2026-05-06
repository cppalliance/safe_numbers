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

void test_multiplication_in_bounds()
{
    const bounded_float<-100.0f, 100.0f> a {f32{4.0f}};
    const bounded_float<-100.0f, 100.0f> b {f32{5.0f}};
    const auto r {a * b};
    BOOST_TEST_EQ(static_cast<float>(r), 20.0f);
}

void test_multiplication_post_op_out_of_range()
{
    const bounded_float<-1.0f, 1.0f> a {f32{0.5f}};
    const bounded_float<-1.0f, 1.0f> b {f32{0.5f}};
    const auto r {a * b};
    BOOST_TEST_EQ(static_cast<float>(r), 0.25f);

    const bounded_float<-10.0f, 10.0f> c {f32{5.0f}};
    const bounded_float<-10.0f, 10.0f> d {f32{5.0f}};
    BOOST_TEST_THROWS((void)(c * d), std::domain_error);
}

void test_multiplication_compound_assignment()
{
    bounded_float<-100.0f, 100.0f> a {f32{4.0f}};
    const bounded_float<-100.0f, 100.0f> b {f32{5.0f}};
    a *= b;
    BOOST_TEST_EQ(static_cast<float>(a), 20.0f);
}

int main()
{
    test_multiplication_in_bounds();
    test_multiplication_post_op_out_of_range();
    test_multiplication_compound_assignment();

    return boost::report_errors();
}

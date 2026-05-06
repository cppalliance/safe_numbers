// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/core/lightweight_test.hpp>

#if BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

#include <boost/safe_numbers.hpp>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace boost::safe_numbers;

void test_round_trip()
{
    bounded_float<-100.0f, 100.0f> val {f32{0.0f}};

    std::stringstream in;
    in.str("42.5");
    in >> val;

    const bounded_float<-100.0f, 100.0f> expected {f32{42.5f}};
    BOOST_TEST(val == expected);

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42.5");
}

void test_negative_round_trip()
{
    bounded_float<-100.0, 100.0> val {f64{0.0}};

    std::stringstream in;
    in.str("-42.5");
    in >> val;

    const bounded_float<-100.0, 100.0> expected {f64{-42.5}};
    BOOST_TEST(val == expected);

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "-42.5");
}

void test_out_of_range_input_throws()
{
    bounded_float<-1.0f, 1.0f> val {f32{0.0f}};

    std::stringstream in;
    in.str("100.0");
    BOOST_TEST_THROWS((in >> val), std::domain_error);
}

void test_fixed_output()
{
    const bounded_float<-100.0f, 100.0f> val {f32{42.5f}};
    std::stringstream out;
    out << std::fixed << std::setprecision(2) << val;
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42.50");
}

int main()
{
    test_round_trip();
    test_negative_round_trip();
    test_out_of_range_input_throws();
    test_fixed_output();

    return boost::report_errors();
}

#else // BOOST_SAFE_NUMBERS_HAS_BOUNDED_FLOAT

int main() { return 0; }

#endif

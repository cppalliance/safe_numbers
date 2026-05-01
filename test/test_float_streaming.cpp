// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/core/lightweight_test.hpp>
#include <iomanip>
#include <limits>
#include <sstream>

using namespace boost::safe_numbers;

template <typename T>
constexpr T make_value(double v) noexcept
{
    return T{static_cast<typename T::basis_type>(v)};
}

// --- Basic round trip ---

template <typename T>
void test()
{
    T val;
    std::stringstream in;
    in.str("42.5");
    in >> val;

    BOOST_TEST_EQ(val, make_value<T>(42.5));

    in.clear();
    const auto endpos {in.tellg()};
    BOOST_TEST_EQ(endpos, 4);

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42.5");
}

template <typename T>
void test_negative_value()
{
    T val;
    std::stringstream in;
    in.str("-42.5");
    in >> val;

    BOOST_TEST_EQ(val, make_value<T>(-42.5));

    in.clear();
    const auto endpos {in.tellg()};
    BOOST_TEST_EQ(endpos, 5);

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "-42.5");
}

template <typename T>
void test_zero_value()
{
    T val;
    std::stringstream in;
    in.str("0");
    in >> val;

    BOOST_TEST_EQ(val, make_value<T>(0.0));

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "0");
}

template <typename T>
void test_whole_number_input()
{
    T val;
    std::stringstream in;
    in.str("42");
    in >> val;

    BOOST_TEST_EQ(val, make_value<T>(42.0));
}

// --- Scientific notation input ---

template <typename T>
void test_scientific_lowercase_input()
{
    T val;
    std::istringstream in("1.5e3");
    in >> val;
    BOOST_TEST_EQ(val, make_value<T>(1500.0));
}

template <typename T>
void test_scientific_uppercase_input()
{
    T val;
    std::istringstream in("1.5E3");
    in >> val;
    BOOST_TEST_EQ(val, make_value<T>(1500.0));
}

template <typename T>
void test_scientific_negative_exponent_input()
{
    T val;
    std::istringstream in("1.5e-2");
    in >> val;
    BOOST_TEST_EQ(val, make_value<T>(0.015));
}

template <typename T>
void test_scientific_negative_value_input()
{
    T val;
    std::istringstream in("-1.5e3");
    in >> val;
    BOOST_TEST_EQ(val, make_value<T>(-1500.0));
}

template <typename T>
void test_scientific_positive_sign_input()
{
    T val;
    std::istringstream in("1.5e+3");
    in >> val;
    BOOST_TEST_EQ(val, make_value<T>(1500.0));
}

// --- Fixed notation output ---

template <typename T>
void test_fixed_output()
{
    std::stringstream out;
    out << std::fixed << std::setprecision(2) << make_value<T>(42.5);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42.50");
}

template <typename T>
void test_fixed_output_zero_precision()
{
    std::stringstream out;
    out << std::fixed << std::setprecision(0) << make_value<T>(42.5);
    // The standard rounds half-to-even at IEEE 754 default, so 42.5 -> 42 or 43.
    // Both are acceptable; libstdc++/libc++ both produce "42" for 42.5.
    BOOST_TEST(out.str() == "42" || out.str() == "43");
}

template <typename T>
void test_fixed_negative_output()
{
    std::stringstream out;
    out << std::fixed << std::setprecision(2) << make_value<T>(-42.5);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "-42.50");
}

// --- Scientific notation output ---

template <typename T>
void test_scientific_output()
{
    std::stringstream out;
    out << std::scientific << std::setprecision(2) << make_value<T>(1500.0);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "1.50e+03");
}

template <typename T>
void test_scientific_uppercase_output()
{
    std::stringstream out;
    out << std::scientific << std::uppercase << std::setprecision(2) << make_value<T>(1500.0);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "1.50E+03");
}

template <typename T>
void test_scientific_nouppercase_output()
{
    std::stringstream out;
    out << std::scientific << std::nouppercase << std::setprecision(2) << make_value<T>(1500.0);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "1.50e+03");
}

template <typename T>
void test_scientific_negative_output()
{
    std::stringstream out;
    out << std::scientific << std::setprecision(2) << make_value<T>(-1500.0);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "-1.50e+03");
}

// --- setprecision (default float format) ---

template <typename T>
void test_setprecision_output()
{
    std::stringstream out;
    out << std::setprecision(3) << make_value<T>(3.14159);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "3.14");
}

template <typename T>
void test_setprecision_high_output()
{
    std::stringstream out;
    out << std::fixed << std::setprecision(6) << make_value<T>(0.5);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "0.500000");
}

// --- showpos / noshowpos ---

template <typename T>
void test_showpos_output()
{
    std::stringstream out;
    out << std::showpos << make_value<T>(42.5);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "+42.5");
}

template <typename T>
void test_noshowpos_output()
{
    std::stringstream out;
    out << std::noshowpos << make_value<T>(42.5);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42.5");
}

template <typename T>
void test_showpos_negative_output()
{
    std::stringstream out;
    out << std::showpos << make_value<T>(-42.5);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "-42.5");
}

// --- showpoint / noshowpoint ---

template <typename T>
void test_showpoint_whole_output()
{
    std::stringstream out;
    out << std::showpoint << std::setprecision(6) << make_value<T>(42.0);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42.0000");
}

template <typename T>
void test_noshowpoint_whole_output()
{
    std::stringstream out;
    out << std::noshowpoint << make_value<T>(42.0);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42");
}

// --- setw / fill ---

template <typename T>
void test_setw_output()
{
    std::stringstream out;
    out << std::setw(8) << make_value<T>(42.5);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "    42.5");
}

template <typename T>
void test_setw_setfill_output()
{
    std::stringstream out;
    out << std::setw(8) << std::setfill('0') << make_value<T>(42.5);
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "000042.5");
}

// --- Special values ---

template <typename T>
void test_infinity_output()
{
    using basis = typename T::basis_type;
    std::stringstream out;
    out << T{std::numeric_limits<basis>::infinity()};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "inf");
}

template <typename T>
void test_negative_infinity_output()
{
    using basis = typename T::basis_type;
    std::stringstream out;
    out << T{-std::numeric_limits<basis>::infinity()};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "-inf");
}

template <typename T>
void test_nan_output()
{
    using basis = typename T::basis_type;
    std::stringstream out;
    out << T{std::numeric_limits<basis>::quiet_NaN()};
    // Some libraries emit "nan", "NaN", or implementation-specific text. All
    // mainstream stdlibs we target produce "nan" with the default flags.
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "nan");
}

// --- Round-trip via string ---

template <typename T>
void test_round_trip_via_string()
{
    const T original {make_value<T>(0.5)};
    std::stringstream s;
    s << original;

    T parsed;
    s >> parsed;
    BOOST_TEST_EQ(parsed, original);
}

template <typename T>
void test_round_trip_negative_via_string()
{
    const T original {make_value<T>(-0.25)};
    std::stringstream s;
    s << original;

    T parsed;
    s >> parsed;
    BOOST_TEST_EQ(parsed, original);
}

int main()
{
    test<f32>();
    test<f64>();

    test_negative_value<f32>();
    test_negative_value<f64>();

    test_zero_value<f32>();
    test_zero_value<f64>();

    test_whole_number_input<f32>();
    test_whole_number_input<f64>();

    // Scientific input
    test_scientific_lowercase_input<f32>();
    test_scientific_lowercase_input<f64>();

    test_scientific_uppercase_input<f32>();
    test_scientific_uppercase_input<f64>();

    test_scientific_negative_exponent_input<f32>();
    test_scientific_negative_exponent_input<f64>();

    test_scientific_negative_value_input<f32>();
    test_scientific_negative_value_input<f64>();

    test_scientific_positive_sign_input<f32>();
    test_scientific_positive_sign_input<f64>();

    // Fixed output
    test_fixed_output<f32>();
    test_fixed_output<f64>();

    test_fixed_output_zero_precision<f32>();
    test_fixed_output_zero_precision<f64>();

    test_fixed_negative_output<f32>();
    test_fixed_negative_output<f64>();

    // Scientific output
    test_scientific_output<f32>();
    test_scientific_output<f64>();

    test_scientific_uppercase_output<f32>();
    test_scientific_uppercase_output<f64>();

    test_scientific_nouppercase_output<f32>();
    test_scientific_nouppercase_output<f64>();

    test_scientific_negative_output<f32>();
    test_scientific_negative_output<f64>();

    // setprecision
    test_setprecision_output<f32>();
    test_setprecision_output<f64>();

    test_setprecision_high_output<f32>();
    test_setprecision_high_output<f64>();

    // showpos / noshowpos
    test_showpos_output<f32>();
    test_showpos_output<f64>();

    test_noshowpos_output<f32>();
    test_noshowpos_output<f64>();

    test_showpos_negative_output<f32>();
    test_showpos_negative_output<f64>();

    // showpoint / noshowpoint
    test_showpoint_whole_output<f32>();
    test_showpoint_whole_output<f64>();

    test_noshowpoint_whole_output<f32>();
    test_noshowpoint_whole_output<f64>();

    // setw / setfill
    test_setw_output<f32>();
    test_setw_output<f64>();

    test_setw_setfill_output<f32>();
    test_setw_setfill_output<f64>();

    // Special values
    test_infinity_output<f32>();
    test_infinity_output<f64>();

    test_negative_infinity_output<f32>();
    test_negative_infinity_output<f64>();

    test_nan_output<f32>();
    test_nan_output<f64>();

    // Round-trip
    test_round_trip_via_string<f32>();
    test_round_trip_via_string<f64>();

    test_round_trip_negative_via_string<f32>();
    test_round_trip_negative_via_string<f64>();

    return boost::report_errors();
}

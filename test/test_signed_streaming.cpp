// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/core/lightweight_test.hpp>
#include <iomanip>
#include <sstream>

using namespace boost::safe_numbers;

template <typename T>
void test()
{
    T val;
    std::stringstream in;
    in.str("42");
    in >> val;

    BOOST_TEST_EQ(val, T{42});

    in.clear();
    const auto endpos {in.tellg()};
    BOOST_TEST_EQ(endpos, 2);

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42");
}

template <typename T>
void test_negative_value()
{
    T val;
    std::stringstream in;
    in.str("-42");
    in >> val;

    BOOST_TEST_EQ(val, T{-42});

    in.clear();
    const auto endpos {in.tellg()};
    BOOST_TEST_EQ(endpos, 3);

    std::stringstream out;
    out << val;

    BOOST_TEST_CSTR_EQ(out.str().c_str(), "-42");
}

// --- Output tests (operator<<) ---

template <typename T>
void test_dec_output()
{
    std::stringstream out;
    out << std::dec << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42");
}

template <typename T>
void test_oct_output()
{
    std::stringstream out;
    out << std::oct << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "52");
}

template <typename T>
void test_hex_output()
{
    std::stringstream out;
    out << std::hex << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "2a");
}

template <typename T>
void test_hex_uppercase_output()
{
    std::stringstream out;
    out << std::hex << std::uppercase << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "2A");
}

template <typename T>
void test_hex_nouppercase_output()
{
    std::stringstream out;
    out << std::hex << std::nouppercase << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "2a");
}

// --- Input tests (operator>>) ---

template <typename T>
void test_dec_input()
{
    T val;
    std::istringstream in("42");
    in >> std::dec >> val;
    BOOST_TEST_EQ(val, T{42});
}

template <typename T>
void test_oct_input()
{
    T val;
    std::istringstream in("52");
    in >> std::oct >> val;
    BOOST_TEST_EQ(val, T{42});
}

template <typename T>
void test_hex_input()
{
    T val;
    std::istringstream in("2a");
    in >> std::hex >> val;
    BOOST_TEST_EQ(val, T{42});
}

template <typename T>
void test_hex_uppercase_input()
{
    T val;
    std::istringstream in("2A");
    in >> std::hex >> std::uppercase >> val;
    BOOST_TEST_EQ(val, T{42});
}

template <typename T>
void test_hex_nouppercase_input()
{
    T val;
    std::istringstream in("2a");
    in >> std::hex >> std::nouppercase >> val;
    BOOST_TEST_EQ(val, T{42});
}

// --- showbase / noshowbase output tests ---

template <typename T>
void test_showbase_dec_output()
{
    std::stringstream out;
    out << std::showbase << std::dec << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42");
}

template <typename T>
void test_showbase_oct_output()
{
    std::stringstream out;
    out << std::showbase << std::oct << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "052");
}

template <typename T>
void test_showbase_hex_output()
{
    std::stringstream out;
    out << std::showbase << std::hex << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "0x2a");
}

template <typename T>
void test_showbase_hex_uppercase_output()
{
    std::stringstream out;
    out << std::showbase << std::hex << std::uppercase << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "0X2A");
}

template <typename T>
void test_noshowbase_dec_output()
{
    std::stringstream out;
    out << std::noshowbase << std::dec << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42");
}

template <typename T>
void test_noshowbase_oct_output()
{
    std::stringstream out;
    out << std::noshowbase << std::oct << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "52");
}

template <typename T>
void test_noshowbase_hex_output()
{
    std::stringstream out;
    out << std::noshowbase << std::hex << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "2a");
}

template <typename T>
void test_noshowbase_hex_uppercase_output()
{
    std::stringstream out;
    out << std::noshowbase << std::hex << std::uppercase << T{42};
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "2A");
}

// --- showbase / noshowbase input tests ---

template <typename T>
void test_showbase_dec_input()
{
    T val;
    std::istringstream in("42");
    in >> std::showbase >> std::dec >> val;
    BOOST_TEST_EQ(val, T{42});
}

template <typename T>
void test_showbase_oct_input()
{
    T val;
    std::istringstream in("52");
    in >> std::showbase >> std::oct >> val;
    BOOST_TEST_EQ(val, T{42});
}

template <typename T>
void test_showbase_hex_input()
{
    T val;
    std::istringstream in("2a");
    in >> std::showbase >> std::hex >> val;
    BOOST_TEST_EQ(val, T{42});
}

template <typename T>
void test_noshowbase_dec_input()
{
    T val;
    std::istringstream in("42");
    in >> std::noshowbase >> std::dec >> val;
    BOOST_TEST_EQ(val, T{42});
}

template <typename T>
void test_noshowbase_oct_input()
{
    T val;
    std::istringstream in("52");
    in >> std::noshowbase >> std::oct >> val;
    BOOST_TEST_EQ(val, T{42});
}

template <typename T>
void test_noshowbase_hex_input()
{
    T val;
    std::istringstream in("2a");
    in >> std::noshowbase >> std::hex >> val;
    BOOST_TEST_EQ(val, T{42});
}

int main()
{
    test<i8>();
    test<i16>();
    test<i32>();
    test<i64>();
    test<i128>();

    test_negative_value<i8>();
    test_negative_value<i16>();
    test_negative_value<i32>();
    test_negative_value<i64>();
    test_negative_value<i128>();

    // Output tests
    test_dec_output<i8>();
    test_dec_output<i16>();
    test_dec_output<i32>();
    test_dec_output<i64>();
    test_dec_output<i128>();

    test_oct_output<i8>();
    test_oct_output<i16>();
    test_oct_output<i32>();
    test_oct_output<i64>();
    test_oct_output<i128>();

    test_hex_output<i8>();
    test_hex_output<i16>();
    test_hex_output<i32>();
    test_hex_output<i64>();
    test_hex_output<i128>();

    test_hex_uppercase_output<i8>();
    test_hex_uppercase_output<i16>();
    test_hex_uppercase_output<i32>();
    test_hex_uppercase_output<i64>();
    test_hex_uppercase_output<i128>();

    test_hex_nouppercase_output<i8>();
    test_hex_nouppercase_output<i16>();
    test_hex_nouppercase_output<i32>();
    test_hex_nouppercase_output<i64>();
    test_hex_nouppercase_output<i128>();

    // Input tests
    test_dec_input<i8>();
    test_dec_input<i16>();
    test_dec_input<i32>();
    test_dec_input<i64>();
    test_dec_input<i128>();

    test_oct_input<i8>();
    test_oct_input<i16>();
    test_oct_input<i32>();
    test_oct_input<i64>();
    test_oct_input<i128>();

    test_hex_input<i8>();
    test_hex_input<i16>();
    test_hex_input<i32>();
    test_hex_input<i64>();
    test_hex_input<i128>();

    test_hex_uppercase_input<i8>();
    test_hex_uppercase_input<i16>();
    test_hex_uppercase_input<i32>();
    test_hex_uppercase_input<i64>();
    test_hex_uppercase_input<i128>();

    test_hex_nouppercase_input<i8>();
    test_hex_nouppercase_input<i16>();
    test_hex_nouppercase_input<i32>();
    test_hex_nouppercase_input<i64>();
    test_hex_nouppercase_input<i128>();

    // showbase output tests
    test_showbase_dec_output<i8>();
    test_showbase_dec_output<i16>();
    test_showbase_dec_output<i32>();
    test_showbase_dec_output<i64>();
    test_showbase_dec_output<i128>();

    test_showbase_oct_output<i8>();
    test_showbase_oct_output<i16>();
    test_showbase_oct_output<i32>();
    test_showbase_oct_output<i64>();
    test_showbase_oct_output<i128>();

    test_showbase_hex_output<i8>();
    test_showbase_hex_output<i16>();
    test_showbase_hex_output<i32>();
    test_showbase_hex_output<i64>();
    test_showbase_hex_output<i128>();

    test_showbase_hex_uppercase_output<i8>();
    test_showbase_hex_uppercase_output<i16>();
    test_showbase_hex_uppercase_output<i32>();
    test_showbase_hex_uppercase_output<i64>();
    test_showbase_hex_uppercase_output<i128>();

    // noshowbase output tests
    test_noshowbase_dec_output<i8>();
    test_noshowbase_dec_output<i16>();
    test_noshowbase_dec_output<i32>();
    test_noshowbase_dec_output<i64>();
    test_noshowbase_dec_output<i128>();

    test_noshowbase_oct_output<i8>();
    test_noshowbase_oct_output<i16>();
    test_noshowbase_oct_output<i32>();
    test_noshowbase_oct_output<i64>();
    test_noshowbase_oct_output<i128>();

    test_noshowbase_hex_output<i8>();
    test_noshowbase_hex_output<i16>();
    test_noshowbase_hex_output<i32>();
    test_noshowbase_hex_output<i64>();
    test_noshowbase_hex_output<i128>();

    test_noshowbase_hex_uppercase_output<i8>();
    test_noshowbase_hex_uppercase_output<i16>();
    test_noshowbase_hex_uppercase_output<i32>();
    test_noshowbase_hex_uppercase_output<i64>();
    test_noshowbase_hex_uppercase_output<i128>();

    // showbase input tests
    test_showbase_dec_input<i8>();
    test_showbase_dec_input<i16>();
    test_showbase_dec_input<i32>();
    test_showbase_dec_input<i64>();
    test_showbase_dec_input<i128>();

    test_showbase_oct_input<i8>();
    test_showbase_oct_input<i16>();
    test_showbase_oct_input<i32>();
    test_showbase_oct_input<i64>();
    test_showbase_oct_input<i128>();

    test_showbase_hex_input<i8>();
    test_showbase_hex_input<i16>();
    test_showbase_hex_input<i32>();
    test_showbase_hex_input<i64>();
    test_showbase_hex_input<i128>();

    // noshowbase input tests
    test_noshowbase_dec_input<i8>();
    test_noshowbase_dec_input<i16>();
    test_noshowbase_dec_input<i32>();
    test_noshowbase_dec_input<i64>();
    test_noshowbase_dec_input<i128>();

    test_noshowbase_oct_input<i8>();
    test_noshowbase_oct_input<i16>();
    test_noshowbase_oct_input<i32>();
    test_noshowbase_oct_input<i64>();
    test_noshowbase_oct_input<i128>();

    test_noshowbase_hex_input<i8>();
    test_noshowbase_hex_input<i16>();
    test_noshowbase_hex_input<i32>();
    test_noshowbase_hex_input<i64>();
    test_noshowbase_hex_input<i128>();

    return boost::report_errors();
}

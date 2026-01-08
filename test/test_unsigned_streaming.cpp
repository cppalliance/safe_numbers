// Copyright 2025 Matt Borland
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

#ifndef BOOST_DISABLE_EXCEPTIONS

template <typename T>
void test_negative_value_handling()
{
    T val;
    std::stringstream in;
    in.str("-42");
    BOOST_TEST_THROWS(in >> val, std::domain_error);
}

#endif

int main()
{
    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();
    test<u128>();

    #ifndef BOOST_DISABLE_EXCEPTIONS

    test_negative_value_handling<u8>();
    test_negative_value_handling<u16>();
    test_negative_value_handling<u32>();
    test_negative_value_handling<u64>();
    test_negative_value_handling<u128>();

    #endif

    return boost::report_errors();
}

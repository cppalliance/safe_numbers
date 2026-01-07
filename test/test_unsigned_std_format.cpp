// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <format>

#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

template <typename T>
void test()
{
    const T x {42};

    BOOST_TEST_CSTR_EQ(std::format("{}", x).c_str(), "42");
    BOOST_TEST_CSTR_EQ(std::format("{:08x}", x).c_str(), "0000002a");
    BOOST_TEST_CSTR_EQ(std::format("{:#010b}", x).c_str(), "0b00101010");
}

int main()
{
    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();

    return boost::report_errors();
}

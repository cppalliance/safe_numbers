// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/unsigned_integers.hpp>

#endif

#include <boost/core/lightweight_test.hpp>

template <typename T>
void test()
{
    const T value {true};
    BOOST_TEST_EQ(value, T{1});
}

int main()
{
    using namespace boost::safe_numbers;

    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();

    return boost::report_errors();
}

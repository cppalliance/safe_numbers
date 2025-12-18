// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;
using namespace boost::safe_numbers::literals;

void test()
{
    constexpr auto u8_value {18446744073709551615_u8};
    constexpr auto u16_value {18446744073709551615_u16};
    constexpr auto u32_value {18446744073709551615_u32};

    static_cast<void>(u8_value);
    static_cast<void>(u16_value);
    static_cast<void>(u32_value);
}

int main()
{
    test();

    return boost::report_errors();
}

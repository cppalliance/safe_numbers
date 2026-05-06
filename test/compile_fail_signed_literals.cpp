// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;
using namespace boost::safe_numbers::literals;

void test()
{
    constexpr auto i8_value {18446744073709551615_i8};
    constexpr auto i16_value {18446744073709551615_i16};
    constexpr auto i32_value {18446744073709551615_i32};
    constexpr auto i64_value {18446744073709551615_i64};
    constexpr auto i128_value {"999999999999999999999999999999999999999"_i128};

    static_cast<void>(i8_value);
    static_cast<void>(i16_value);
    static_cast<void>(i32_value);
    static_cast<void>(i64_value);
    static_cast<void>(i128_value);
}

int main()
{
    test();

    return boost::report_errors();
}

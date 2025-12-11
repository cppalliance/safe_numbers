// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// This file can be used in conjunction with the pretty printers
// to see how various values will be represented

#include <boost/safe_numbers/unsigned_integers.hpp>     // For safe_numbers types
#include <boost/safe_numbers/limits.hpp>                // For safe_numbers <limits> support

template <typename T>
void debug_values()
{
    const T max {std::numeric_limits<T>::max()};
    const T min {std::numeric_limits<T>::min()};

    // Break Here:
    static_cast<void>(max);
    static_cast<void>(min);
}

int main()
{
    debug_values<boost::safe_numbers::u8>();
    debug_values<boost::safe_numbers::u16>();
    debug_values<boost::safe_numbers::u32>();
    debug_values<boost::safe_numbers::u64>();

    return 0;
}

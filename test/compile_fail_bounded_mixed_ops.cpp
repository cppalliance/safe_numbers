// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>

using namespace boost::safe_numbers;

int main()
{
    constexpr bounded_uint<0u, 255u> a {u8{10}};
    constexpr bounded_uint<0u, 100u> b {u8{10}};

    // This should fail to compile: different bounds
    auto c = a + b;

    return 0;
}

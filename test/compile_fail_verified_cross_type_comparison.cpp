// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>

using namespace boost::safe_numbers;

int main()
{
    constexpr auto a = verified_u32{u32{10}};
    constexpr auto b = verified_u64{u64{10}};

    // This should fail to compile: can not compare verified types
    // with different basis types
    auto result = (a == b);

    return 0;
}

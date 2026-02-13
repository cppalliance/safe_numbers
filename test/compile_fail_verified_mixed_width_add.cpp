// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>

using namespace boost::safe_numbers;

int main()
{
    constexpr auto a = verified_u32{u32{10}};
    const auto b = u64{20};

    // This should fail to compile: verified_u32 + u64 is a mixed-width operation
    auto c = a + b;

    return 0;
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/floats.hpp>

using namespace boost::safe_numbers;

int main()
{
    f32 a {1.0f};
    f64 b {2.0};

    // Mixed f32 and f64 operations are formally blocked with a static_assert
    auto c = a + b;
    static_cast<void>(c);

    return 0;
}

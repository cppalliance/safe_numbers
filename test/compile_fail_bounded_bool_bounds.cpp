// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>

using namespace boost::safe_numbers;

int main()
{
    // bool bounds should be rejected
    bounded_uint<false, true> a {u8{0}};

    return 0;
}

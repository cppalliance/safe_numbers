//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"

int main()
{
    return sn_sycl_test::run_all_unsigned("mod",
        [](auto a, auto b) { return a % sn_sycl_test::nonzero(b); });
}

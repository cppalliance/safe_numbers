//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"

int main()
{
    return sn_sycl_test::run_unary_all_unsigned<sn_sycl_test::gen_kind::full>("bit_floor", [](auto a) { return boost::safe_numbers::bit_floor(a); });
}

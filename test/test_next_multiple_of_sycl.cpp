//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"

int main()
{
    return sn_sycl_test::run_binary_all_unsigned<sn_sycl_test::gen_kind::below_half_max, sn_sycl_test::gen_kind::nonzero>("next_multiple_of", [](auto a, auto b) { return boost::safe_numbers::next_multiple_of(a, b); });
}

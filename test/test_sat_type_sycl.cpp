//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

// Smoke test for the saturating type-level policy on SYCL devices: full-range
// operands so a large fraction of the additions actually saturate on device

#include "sycl_test.hpp"

int main()
{
    using boost::safe_numbers::sat_u32;
    using boost::safe_numbers::sat_i32;
    using boost::safe_numbers::sat_f32;

    int rc {EXIT_SUCCESS};

    rc |= sn_sycl_test::run_binary<sat_u32>("sat_u32 add", &sn_sycl_test::draw_full<sat_u32>,
                                            [](auto a, auto b) { return a + b; });
    rc |= sn_sycl_test::run_binary<sat_u32>("sat_u32 mul", &sn_sycl_test::draw_full<sat_u32>,
                                            [](auto a, auto b) { return a * b; });
    rc |= sn_sycl_test::run_binary<sat_i32>("sat_i32 add", &sn_sycl_test::draw_full<sat_i32>,
                                            [](auto a, auto b) { return a + b; });
    rc |= sn_sycl_test::run_binary<sat_i32>("sat_i32 sub", &sn_sycl_test::draw_full<sat_i32>,
                                            [](auto a, auto b) { return a - b; });
    rc |= sn_sycl_test::run_binary<sat_f32>("sat_f32 add",
                                            [](std::mt19937_64& rng) { return sn_sycl_test::draw_real<sat_f32>(rng, -1.0e30, 1.0e30); },
                                            [](auto a, auto b) { return a + b; });

    return rc;
}

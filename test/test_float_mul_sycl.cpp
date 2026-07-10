//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  Multiplication on the device for f32 and f64 over random finite operands,
//  verified exactly against a host recomputation.

#include "sycl_test.hpp"

template <typename T>
int run(const char* label)
{
    return sn_sycl_test::run_binary_2<T>(label,
        [](std::mt19937_64& rng) { return sn_sycl_test::draw_real<T>(rng, -1e15, 1e15); },
        [](std::mt19937_64& rng) { return sn_sycl_test::draw_real<T>(rng, -1e15, 1e15); },
        [](auto a, auto b) { return a * b; });
}

int main()
{
    using namespace boost::safe_numbers;

    int rc {EXIT_SUCCESS};
    rc |= run<f32>("f32 mul");
    rc |= run<f64>("f64 mul");
    return rc;
}

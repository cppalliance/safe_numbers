//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  Signed arithmetic error paths (no CUDA analog exists for these, but the
//  signed family has error conditions the unsigned matrix cannot reach):
//  add overflow/underflow, division and modulo by zero, the min / -1
//  overflow, and unary negation of min.

#include "sycl_test.hpp"
#include <limits>

template <typename T>
int run(const char* name)
{
    using basis_type = typename T::basis_type;
    constexpr auto max_val {(std::numeric_limits<basis_type>::max)()};
    constexpr auto min_val {(std::numeric_limits<basis_type>::min)()};

    int rc {EXIT_SUCCESS};

    rc |= sn_sycl_test::expect_binary_throw<std::overflow_error, T>(name,
        T{max_val}, T{max_val},
        [](auto x, auto y) { return x + y; });

    rc |= sn_sycl_test::expect_binary_throw<std::underflow_error, T>(name,
        T{min_val}, T{min_val},
        [](auto x, auto y) { return x + y; });

    rc |= sn_sycl_test::expect_binary_throw<std::domain_error, T>(name,
        T{basis_type{1}}, T{basis_type{0}},
        [](auto x, auto y) { return x / y; });

    rc |= sn_sycl_test::expect_binary_throw<std::overflow_error, T>(name,
        T{min_val}, T{static_cast<basis_type>(-1)},
        [](auto x, auto y) { return x / y; });

    rc |= sn_sycl_test::expect_binary_throw<std::domain_error, T>(name,
        T{basis_type{1}}, T{basis_type{0}},
        [](auto x, auto y) { return x % y; });

    rc |= sn_sycl_test::expect_binary_throw<std::overflow_error, T>(name,
        T{min_val}, T{static_cast<basis_type>(-1)},
        [](auto x, auto y) { return x % y; });

    rc |= sn_sycl_test::expect_binary_throw<std::domain_error, T>(name,
        T{min_val}, T{basis_type{0}},
        [](auto x, auto y) { static_cast<void>(y); return -x; });

    return rc;
}

int main()
{
    using namespace boost::safe_numbers;

    int rc {EXIT_SUCCESS};
    rc |= run<i8>("i8");
    rc |= run<i64>("i64");
    rc |= run<i128>("i128");
    return rc;
}

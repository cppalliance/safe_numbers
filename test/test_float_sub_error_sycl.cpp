//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  A float subtraction that saturates to -infinity must be reported from the device and surface on the host as
//  std::underflow_error via device_error_context::synchronize().

#include "sycl_test.hpp"
#include <limits>

template <typename T>
int run(const char* label)
{
    using basis_type = typename T::basis_type;

    return sn_sycl_test::expect_binary_throw<std::underflow_error, T>(label,
        T{std::numeric_limits<basis_type>::lowest()}, T{(std::numeric_limits<basis_type>::max)()},
        [](auto x, auto y) { return x - y; });
}

int main()
{
    using namespace boost::safe_numbers;

    int rc {EXIT_SUCCESS};
    rc |= run<f32>("f32 sub");
    rc |= run<f64>("f64 sub");
    return rc;
}

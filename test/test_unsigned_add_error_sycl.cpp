//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  An unsigned addition that overflows must be reported from the device and surface on the host as
//  std::overflow_error via device_error_context::synchronize().

#include "sycl_test.hpp"
#include <limits>

template <typename T>
int run(const char* label)
{
    using basis_type = typename T::basis_type;

    return sn_sycl_test::expect_binary_throw<std::overflow_error, T>(label,
        T{(std::numeric_limits<basis_type>::max)()}, T{(std::numeric_limits<basis_type>::max)()},
        [](auto x, auto y) { return x + y; });
}

int main()
{
    using namespace boost::safe_numbers;

    int rc {EXIT_SUCCESS};
    rc |= run<u8>("u8 add");
    rc |= run<u16>("u16 add");
    rc |= run<u32>("u32 add");
    rc |= run<u64>("u64 add");
    rc |= run<u128>("u128 add");
    return rc;
}

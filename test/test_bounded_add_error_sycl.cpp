//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  A bounded addition whose result leaves the declared range must be reported
//  from the device and surface on the host as std::domain_error, for all
//  three bounded families.

#include "sycl_test.hpp"
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/bounded_floats.hpp>

int main()
{
    using bu = boost::safe_numbers::bounded_uint<0U, 1000U>;
    using bi = boost::safe_numbers::bounded_int<-1000, 1000>;
    using bf = boost::safe_numbers::bounded_float<0.0F, 1000.0F>;

    int rc {EXIT_SUCCESS};

    // 600 + 600 = 1200 exceeds every declared range below
    rc |= sn_sycl_test::expect_binary_throw<std::domain_error, bu>(
        "bounded_uint add out of range",
        bu{600U}, bu{600U},
        [](auto x, auto y) { return x + y; });

    rc |= sn_sycl_test::expect_binary_throw<std::domain_error, bi>(
        "bounded_int add out of range",
        bi{600}, bi{600},
        [](auto x, auto y) { return x + y; });

    rc |= sn_sycl_test::expect_binary_throw<std::domain_error, bf>(
        "bounded_float add out of range",
        bf{600.0F}, bf{600.0F},
        [](auto x, auto y) { return x + y; });

    return rc;
}

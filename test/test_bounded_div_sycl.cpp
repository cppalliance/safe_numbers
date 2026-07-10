//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  In-range bounded division on the device for all three bounded families,
//  verified exactly against a host recomputation. Operand ranges keep every
//  result inside the declared bounds.

#include "sycl_test.hpp"
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/bounded_floats.hpp>

using bu = boost::safe_numbers::bounded_uint<0U, 1000000U>;
using bi = boost::safe_numbers::bounded_int<-1000000, 1000000>;
using bf = boost::safe_numbers::bounded_float<0.0F, 1000000.0F>;

// Uniform draw in [lo, hi] constructed through the bounded type's underlying type
template <typename BoundedT, typename Raw>
BoundedT draw(std::mt19937_64& rng, const long long lo, const long long hi)
{
    const auto span {static_cast<std::uint64_t>(hi - lo + 1)};
    return BoundedT{static_cast<Raw>(lo + static_cast<long long>(rng() % span))};
}

int main()
{
    const auto op = [](auto a, auto b) { return a / b; };

    int rc {EXIT_SUCCESS};

    rc |= sn_sycl_test::run_binary_2<bu>("bounded_uint div",
        [](std::mt19937_64& rng) { return draw<bu, std::uint32_t>(rng, 0, 1000000); },
        [](std::mt19937_64& rng) { return draw<bu, std::uint32_t>(rng, 1, 1000); },
        op);

    rc |= sn_sycl_test::run_binary_2<bi>("bounded_int div",
        [](std::mt19937_64& rng) { return draw<bi, std::int32_t>(rng, -1000000, 1000000); },
        [](std::mt19937_64& rng) { return draw<bi, std::int32_t>(rng, 1, 1000); },
        op);

    rc |= sn_sycl_test::run_binary_2<bf>("bounded_float div",
        [](std::mt19937_64& rng) { return draw<bf, float>(rng, 0, 1000000); },
        [](std::mt19937_64& rng) { return draw<bf, float>(rng, 1, 1000); },
        op);

    return rc;
}

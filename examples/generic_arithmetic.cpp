// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example demonstrates the generic policy-parameterized arithmetic
// functions (add, sub, mul, div, mod). These accept an overflow_policy
// as a template parameter, allowing you to write code that is generic
// over the overflow handling strategy.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>

int main()
{
    using boost::safe_numbers::u32;
    using boost::safe_numbers::overflow_policy;
    using boost::safe_numbers::add;
    using boost::safe_numbers::sub;
    using boost::safe_numbers::mul;

    const u32 a {100U};
    const u32 b {50U};

    // Same operation, different policies via the generic interface
    {
        const auto throwing  {add<overflow_policy::throw_exception>(a, b)};
        const auto saturated {add<overflow_policy::saturate>(a, b)};
        const auto wrapped   {add<overflow_policy::wrapping>(a, b)};
        const auto strict    {add<overflow_policy::strict>(a, b)};

        std::cout << "add<throw_exception>(100, 50) = " << throwing << std::endl;
        std::cout << "add<saturate>(100, 50)        = " << saturated << std::endl;
        std::cout << "add<wrapping>(100, 50)        = " << wrapped << std::endl;
        std::cout << "add<strict>(100, 50)          = " << strict << std::endl;
    }

    // Policies with different return types
    {
        // overflow_tuple returns std::pair<T, bool>
        const auto [result_ot, overflowed] {add<overflow_policy::overflow_tuple>(a, b)};
        std::cout << "add<overflow_tuple>(100, 50)   = " << result_ot
                  << " (overflowed: " << std::boolalpha << overflowed << ")" << std::endl;

        // checked returns std::optional<T>
        const auto result_chk {add<overflow_policy::checked>(a, b)};
        if (result_chk)
        {
            std::cout << "add<checked>(100, 50)          = " << *result_chk << std::endl;
        }
    }

    // The real power: writing generic algorithms parameterized on policy
    {
        const u32 max_val {std::numeric_limits<std::uint32_t>::max()};
        const u32 one {1U};

        // checked policy returns nullopt on overflow
        const auto checked_result {add<overflow_policy::checked>(max_val, one)};
        std::cout << "add<checked>(max, 1)           = "
                  << (checked_result ? "has value" : "nullopt (overflow)")
                  << std::endl;

        // saturate policy clamps to max
        const auto sat_result {add<overflow_policy::saturate>(max_val, one)};
        std::cout << "add<saturate>(max, 1)          = " << sat_result << std::endl;

        // wrapping policy wraps around
        const auto wrap_result {add<overflow_policy::wrapping>(max_val, one)};
        std::cout << "add<wrapping>(max, 1)          = " << wrap_result << std::endl;
    }

    return 0;
}


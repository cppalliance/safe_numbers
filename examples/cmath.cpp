// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/cmath.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <limits>
#include <stdexcept>

int main()
{
    using namespace boost::safe_numbers;

    std::cout << std::boolalpha;

    // Value functions return the same safe floating-point type
    std::cout << "sqrt(4.0)      = " << sqrt(f64{4.0}) << '\n';
    std::cout << "hypot(3, 4)    = " << hypot(f64{3.0}, f64{4.0}) << '\n';
    std::cout << "pow(2, 10)     = " << pow(f64{2.0}, f64{10.0}) << '\n';

    std::cout << '\n';

    // Classification predicates never throw, even on non-finite values
    const f64 inf {std::numeric_limits<double>::infinity()};
    const f64 nan {std::numeric_limits<double>::quiet_NaN()};
    std::cout << "isinf(inf)     = " << isinf(inf) << '\n';
    std::cout << "isnan(nan)     = " << isnan(nan) << '\n';
    std::cout << "isfinite(1.0)  = " << isfinite(f64{1.0}) << '\n';

    std::cout << '\n';

    // Non-signaling comparisons are false when an operand is NaN
    std::cout << "isless(1, 2)   = " << isless(f64{1.0}, f64{2.0}) << '\n';
    std::cout << "isless(nan, 2) = " << isless(nan, f64{2.0}) << '\n';

    std::cout << '\n';

    // Integer-returning functions produce a safe integer type
    std::cout << "lround(2.5)    = " << lround(f64{2.5}) << '\n';
    std::cout << "ilogb(8.0)     = " << ilogb(f64{8.0}) << '\n';

    std::cout << '\n';

    // Exceptional results raise an exception instead of propagating
    try
    {
        const auto bad {sqrt(f64{-1.0})};
        static_cast<void>(bad);
    }
    catch (const std::domain_error&)
    {
        std::cout << "sqrt(-1.0) threw std::domain_error\n";
    }

    try
    {
        const auto big {pow(f64{2.0}, f64{2000.0})};
        static_cast<void>(big);
    }
    catch (const std::overflow_error&)
    {
        std::cout << "pow(2, 2000) threw std::overflow_error\n";
    }

    return 0;
}

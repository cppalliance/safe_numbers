// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/floats.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <limits>
#include <stdexcept>

int main()
{
    using namespace boost::safe_numbers;

    // Construction is explicit from the exact underlying type
    const f64 a {3.0};
    const f64 b {4.0};

    // The four checked operators return the same safe type
    std::cout << "a + b = " << (a + b) << '\n';
    std::cout << "a - b = " << (a - b) << '\n';
    std::cout << "a * b = " << (a * b) << '\n';
    std::cout << "a / b = " << (a / b) << '\n';

    std::cout << '\n';

    // Conversion back to the underlying type is explicit
    const double raw {static_cast<double>(a * b)};
    std::cout << "static_cast<double>(a * b) = " << raw << '\n';

    std::cout << '\n';

    // Comparison uses operator<=> (std::partial_ordering); any NaN is unordered
    const f64 nan {std::numeric_limits<double>::quiet_NaN()};
    std::cout << std::boolalpha;
    std::cout << "a < b        = " << (a < b) << '\n';
    std::cout << "nan < b      = " << (nan < b) << '\n';
    std::cout << "nan == nan   = " << (nan == nan) << '\n';

    std::cout << '\n';

    // Exceptional IEEE 754 results become exceptions
    const f64 big {std::numeric_limits<double>::max()};
    const f64 neg_big {-std::numeric_limits<double>::max()};

    try
    {
        const auto over {big + big};        // saturates to +infinity
        static_cast<void>(over);
    }
    catch (const std::overflow_error&)
    {
        std::cout << "max + max threw std::overflow_error\n";
    }

    try
    {
        const auto under {neg_big + neg_big};   // saturates to -infinity
        static_cast<void>(under);
    }
    catch (const std::underflow_error&)
    {
        std::cout << "(-max) + (-max) threw std::underflow_error\n";
    }

    try
    {
        const auto dbz {a / f64{0.0}};      // finite non-zero divided by zero
        static_cast<void>(dbz);
    }
    catch (const std::domain_error&)
    {
        std::cout << "a / 0.0 threw std::domain_error\n";
    }

    try
    {
        const auto with_nan {nan + a};      // an operand is NaN
        static_cast<void>(with_nan);
    }
    catch (const std::domain_error&)
    {
        std::cout << "nan + a threw std::domain_error\n";
    }

    return 0;
}

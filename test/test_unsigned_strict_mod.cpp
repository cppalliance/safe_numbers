// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test verifies that strict_mod calls std::exit(EXIT_FAILURE) on modulo by zero
// It is marked as run-fail in the Jamfile

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

using namespace boost::safe_numbers;

int main()
{
    // Create values for modulo by zero
    constexpr u32 one {1U};
    constexpr u32 zero {0U};

    // This should call std::exit(EXIT_FAILURE)
    const auto result {strict_mod(one, zero)};

    // Should never reach here
    static_cast<void>(result);
    return 0;
}

// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test verifies that strict_add calls std::exit(EXIT_FAILURE) on overflow
// It is marked as run-fail in the Jamfile

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <limits>

#endif

using namespace boost::safe_numbers;

int main()
{
    // Create values that will overflow when added
    constexpr u32 max_val {std::numeric_limits<u32>::max()};
    constexpr u32 one {1U};

    // This should call std::exit(EXIT_FAILURE)
    const auto result {strict_add(max_val, one)};

    // Should never reach here
    static_cast<void>(result);
    return 0;
}

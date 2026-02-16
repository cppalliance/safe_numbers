// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test verifies that strict_sub calls std::exit(EXIT_FAILURE) on underflow
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
    // Create values that will underflow when subtracted
    constexpr u32 zero {0U};
    constexpr u32 one {1U};

    // This should call std::exit(EXIT_FAILURE)
    const auto result {strict_sub(zero, one)};

    // Should never reach here
    // We don't use BOOST_SAFE_NUMBERS_UNREACHABLE because run-fail is what we are testing for
    static_cast<void>(result);
    return 0;   // LCOV_EXCL_LINE
}

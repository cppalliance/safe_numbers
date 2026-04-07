// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test verifies that strict_mod calls std::exit(EXIT_FAILURE) on overflow
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
    // min % -1 overflows for signed integers
    constexpr i32 min_val {std::numeric_limits<i32>::min()};
    constexpr i32 neg_one {-1};

    // This should call std::exit(EXIT_FAILURE)
    const auto result {strict_mod(min_val, neg_one)};

    // Should never reach here
    static_cast<void>(result);
    return 0;   // LCOV_EXCL_LINE
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test verifies that strict_u32 addition calls std::exit(EXIT_FAILURE) on overflow
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
    const strict_u32 max_val {std::numeric_limits<std::uint32_t>::max()};
    const strict_u32 one {1U};

    // This should call std::exit(EXIT_FAILURE)
    const auto result {max_val + one};

    // Should never reach here
    static_cast<void>(result);
    return 0;   // LCOV_EXCL_LINE
}

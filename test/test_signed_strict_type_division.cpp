// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This test verifies that strict_i32 division by zero calls std::exit(EXIT_FAILURE)
// It is marked as run-fail in the Jamfile

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

using namespace boost::safe_numbers;

int main()
{
    const strict_i32 val {1};
    const strict_i32 zero {0};

    // This should call std::exit(EXIT_FAILURE)
    const auto result {val / zero};

    // Should never reach here
    static_cast<void>(result);
    return 0;   // LCOV_EXCL_LINE
}

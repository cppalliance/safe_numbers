// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// strict is an integer only policy: std::exit has no defined meaning for the
// IEEE 754 error taxonomy the float types use

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/floats.hpp>

#endif

using namespace boost::safe_numbers;

int main()
{
    const detail::float_basis<float, overflow_policy::strict> bad {1.0F};

    return static_cast<int>(static_cast<float>(bad));
}

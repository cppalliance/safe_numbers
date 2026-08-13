// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Arithmetic between the same width with different overflow policies must not compile

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <cstdint>

#endif

using namespace boost::safe_numbers;

int main()
{
    const sat_u8 lhs {1U};
    const u8 rhs {1U};

    const auto res {lhs + rhs};

    return static_cast<int>(static_cast<std::uint8_t>(res));
}

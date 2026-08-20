// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// The tag types select built-in policies through the basic_* alias templates
// only; they are not handlers and can not be used as the basis NTTP directly

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <cstdint>

#endif

using namespace boost::safe_numbers;

int main()
{
    const detail::unsigned_integer_basis<std::uint8_t, saturating{}> bad {1U};

    return static_cast<int>(static_cast<std::uint8_t>(bad));
}

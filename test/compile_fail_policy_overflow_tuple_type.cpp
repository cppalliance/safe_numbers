// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// overflow_tuple returns std::pair so it can not be a type-level policy

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/signed_integers.hpp>
#include <cstdint>

#endif

using namespace boost::safe_numbers;

int main()
{
    const detail::signed_integer_basis<std::int8_t, overflow_policy::overflow_tuple> bad {std::int8_t{1}};

    return static_cast<int>(static_cast<std::int8_t>(bad));
}

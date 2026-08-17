// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// A user defined handler must provide
// on_error(error_kind, BasisType, const char*) returning BasisType

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/unsigned_integers.hpp>

#endif

using namespace boost::safe_numbers;

struct bad_signature
{
    template <typename T>
    constexpr auto on_error(const T value) const noexcept -> T
    {
        return value;
    }
};

int main()
{
    const basic_u8<bad_signature> bad {1U};

    return static_cast<int>(static_cast<std::uint8_t>(bad));
}

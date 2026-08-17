// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// A user defined handler must be stateless: the policy is part of the type,
// so per-value state would silently be lost

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/unsigned_integers.hpp>

#endif

using namespace boost::safe_numbers;

struct stateful_handler
{
    int error_count {0};

    template <typename T>
    constexpr auto on_error(const error_kind, const T value, const char*) const noexcept -> T
    {
        return value;
    }
};

int main()
{
    const basic_u8<stateful_handler> bad {1U};

    return static_cast<int>(static_cast<std::uint8_t>(bad));
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Handler-typed values do not mix with other policies, exactly like the
// built-in policies do not mix with each other

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/unsigned_integers.hpp>

#endif

using namespace boost::safe_numbers;

struct wrap_handler
{
    template <typename T>
    constexpr auto on_error(const error_kind, const T value, const char*) const noexcept -> T
    {
        return value;
    }
};

int main()
{
    const auto res {basic_u8<wrap_handler>{1U} + sat_u8{1U}};

    return static_cast<int>(static_cast<std::uint8_t>(res));
}

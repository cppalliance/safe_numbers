// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example demonstrates user defined error handlers.
// A handler is a stateless type whose on_error decides the result of a failed
// operation; it is selected through the basic_* alias templates, so no detail
// machinery or underlying type ever appears in user code.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/floats.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <limits>
#include <cstdint>

// Returning the fallback value unchanged gives wrapping semantics,
// like Rust's Wrapping<T>
struct wrapping_handler
{
    template <typename T>
    constexpr auto on_error(const boost::safe_numbers::error_kind,
                            const T value,
                            const char*) const noexcept -> T
    {
        return value;
    }
};

// Handlers can also throw their own exception types with the library message
struct logging_handler
{
    template <typename T>
    auto on_error(const boost::safe_numbers::error_kind,
                  const T value,
                  const char* msg) const -> T
    {
        std::cerr << "recovered: " << msg << "\n";
        return value;
    }
};

int main()
{
    using boost::safe_numbers::basic_u8;
    using boost::safe_numbers::basic_f32;
    using boost::safe_numbers::saturating;
    using boost::safe_numbers::u8;
    using boost::safe_numbers::sat_u8;

    // The tags select the built-in policies through the same spelling:
    // basic_u8<> is u8 and basic_u8<saturating> is sat_u8
    static_assert(std::is_same_v<basic_u8<>, u8>);
    static_assert(std::is_same_v<basic_u8<saturating>, sat_u8>);

    using wrap_u8 = basic_u8<wrapping_handler>;
    using log_u8 = basic_u8<logging_handler>;

    // Wrapping arithmetic: no exception, two's complement wrap
    {
        wrap_u8 counter {250U};
        counter += wrap_u8{10U};

        std::cout << "wrap_u8{250} += 10 = " << counter << std::endl;
        // Output: 4
    }

    // Wrapping works in constant expressions because the result is defined
    {
        constexpr wrap_u8 wrapped {wrap_u8{255U} + wrap_u8{1U}};
        static_assert(static_cast<std::uint8_t>(wrapped) == 0U);

        std::cout << "constexpr wrap_u8{255} + wrap_u8{1} = " << wrapped << std::endl;
        // Output: 0
    }

    // A handler observes the error kind and the library's diagnostic message
    {
        const log_u8 big {200U};
        const auto result {big + big};

        std::cout << "log_u8{200} + log_u8{200} = " << result << std::endl;
        // Output on stderr: recovered: Overflow detected in u8 addition
        // Output: 144
    }

    // Float handlers receive the raw IEEE 754 result
    {
        using ieee_f32 = basic_f32<wrapping_handler>;
        const ieee_f32 big {std::numeric_limits<float>::max()};

        std::cout << "ieee_f32{FLT_MAX} + ieee_f32{FLT_MAX} = " << big + big << std::endl;
        // Output: inf
    }

    return 0;
}

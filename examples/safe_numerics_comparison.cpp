// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example compares the behavior of Boost.SafeNumbers unsigned integer
// types against Boost.SafeNumerics for common unsigned arithmetic scenarios.
//
// Key differences:
//   1. SafeNumbers always operates at the declared width - u8 + u8 stays u8.
//      SafeNumerics with the default "native" promotion policy follows C++
//      integer promotion rules, so safe<uint8_t> + safe<uint8_t> silently
//      promotes to int. This means many overflow cases go undetected.
//   2. SafeNumbers requires explicit construction and forbids implicit
//      conversions; SafeNumerics allows implicit construction from built-ins.
//   3. SafeNumbers provides named free functions for alternative overflow
//      policies (wrapping_add, saturating_sub, checked_mul, etc.);
//      SafeNumerics selects behavior via template policy parameters on the type.
//   4. SafeNumbers forbids mixed-width arithmetic at compile time;
//      SafeNumerics promotes operands using C++ native promotion rules.
//   5. SafeNumbers forbids unary minus on unsigned types at compile time;
//      SafeNumerics promotes and allows it silently.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <boost/safe_numerics/safe_integer.hpp>

#include <iostream>
#include <cstdint>

int main()
{
    namespace safe_num = boost::safe_numbers;
    namespace safe_nrc = boost::safe_numerics;

    // -----------------------------------------------------------------------
    // 1. Construction
    // -----------------------------------------------------------------------
    std::cout << "--- Construction ---\n";

    // SafeNumbers: explicit construction only
    {
        const auto x = safe_num::u8{200U};
        std::cout << "safe_numbers explicit: u8{200} = " << x << std::endl;
    }

    // SafeNumerics: implicit construction from built-in types is allowed
    {
        safe_nrc::safe<std::uint8_t> x = 200;
        std::cout << "safe_numerics implicit: u8 = 200 -> "
                  << static_cast<int>(x) << std::endl;
    }

    // SafeNumbers forbids construction from bool (compile error):
    //   safe_num::u8 from_bool{true};  // static_assert failure

    // SafeNumerics allows it:
    {
        safe_nrc::safe<std::uint8_t> from_bool = true;  // OK, value is 1
        std::cout << "safe_numerics from bool: " << static_cast<int>(from_bool) << std::endl;
    }

    // -----------------------------------------------------------------------
    // 2. Overflow detection - the critical difference
    // -----------------------------------------------------------------------
    // SafeNumbers always operates at the declared width.
    // SafeNumerics with "native" policy follows C++ integer promotion:
    //   safe<uint8_t> + safe<uint8_t> promotes to int, so 255 + 1 = 256
    //   which fits in int - NO exception is thrown!
    std::cout << "\n--- Overflow on addition ---\n";

    // SafeNumbers: u8 + u8 stays u8, 255 + 1 overflows -> throws
    try
    {
        const auto result = safe_num::u8{255U} + safe_num::u8{1U};
        std::cout << "safe_numbers: " << result << std::endl;
    }
    catch (const std::overflow_error& e)
    {
        std::cout << "safe_numbers threw: " << e.what() << std::endl;
    }

    // SafeNumerics: uint8_t promotes to int, so 255 + 1 = 256, no overflow
    try
    {
        safe_nrc::safe<std::uint8_t> x = 255;
        safe_nrc::safe<std::uint8_t> y = 1;
        auto result = x + y;  // result is safe<int> with value 256
        std::cout << "safe_numerics: " << static_cast<int>(result) << std::endl;
        // Output: 256 (promoted to int, no overflow detected!)
    }
    catch (const std::exception& e)
    {
        std::cout << "safe_numerics threw: " << e.what() << std::endl;
    }

    // -----------------------------------------------------------------------
    // 3. Underflow detection on subtraction
    // -----------------------------------------------------------------------
    // Same promotion issue: 0u8 - 1u8 promotes to int, giving -1
    std::cout << "\n--- Underflow on subtraction ---\n";

    // SafeNumbers: u8 - u8 stays u8, 0 - 1 underflows -> throws
    try
    {
        const auto result = safe_num::u8{0U} - safe_num::u8{1U};
        std::cout << "safe_numbers: " << result << std::endl;
    }
    catch (const std::underflow_error& e)
    {
        std::cout << "safe_numbers threw: " << e.what() << std::endl;
    }

    // SafeNumerics: uint8_t promotes to int, so 0 - 1 = -1, no underflow
    try
    {
        safe_nrc::safe<std::uint8_t> x = 0;
        safe_nrc::safe<std::uint8_t> y = 1;
        auto result = x - y;  // result is safe<int> with value -1
        std::cout << "safe_numerics: " << static_cast<int>(result) << std::endl;
        // Output: -1 (promoted to int, no underflow detected!)
    }
    catch (const std::exception& e)
    {
        std::cout << "safe_numerics threw: " << e.what() << std::endl;
    }

    // -----------------------------------------------------------------------
    // 4. Alternative overflow policies (SafeNumbers only)
    // -----------------------------------------------------------------------
    // SafeNumbers provides named free functions for each policy.
    // SafeNumerics has no equivalent - behavior is fixed at the type level.
    std::cout << "\n--- Alternative policies (SafeNumbers) ---\n";

    {
        const auto x = safe_num::u8{250U};
        const auto y = safe_num::u8{10U};

        // Wrapping: modular arithmetic like built-in unsigned
        const auto wrapped = safe_num::wrapping_add(x, y);
        std::cout << "wrapping_add(250, 10) = " << wrapped << std::endl;
        // Output: 4

        // Saturating: clamp at the max/min boundary
        const auto saturated = safe_num::saturating_add(x, y);
        std::cout << "saturating_add(250, 10) = " << saturated << std::endl;
        // Output: 255

        // Checked: returns std::optional (nullopt on overflow)
        const auto checked = safe_num::checked_add(x, y);
        std::cout << "checked_add(250, 10) = "
                  << (checked.has_value() ? "value" : "nullopt") << std::endl;
        // Output: nullopt

        // Overflowing: returns {result, overflowed_flag}
        const auto [val, overflowed] = safe_num::overflowing_add(x, y);
        std::cout << "overflowing_add(250, 10) = {" << val
                  << ", " << (overflowed ? "true" : "false") << "}" << std::endl;
        // Output: {4, true}
    }

    // -----------------------------------------------------------------------
    // 5. Mixed-width arithmetic
    // -----------------------------------------------------------------------
    std::cout << "\n--- Mixed-width arithmetic ---\n";

    // SafeNumbers: mixed-width operations are a compile-time error
    //   safe_num::u8{1U} + safe_num::u32{2U};  // fails with static_assert

    // SafeNumerics: mixed-width operations are allowed with C++ promotion rules
    {
        safe_nrc::safe<std::uint8_t> x = 200;
        safe_nrc::safe<std::uint32_t> y = 100;
        auto result = x + y;  // promoted to safe<uint32_t>
        std::cout << "safe_numerics mixed: u8(200) + u32(100) = "
                  << static_cast<std::uint32_t>(result) << std::endl;
        // Output: 300
    }

    // SafeNumbers requires explicit conversion to the same type first:
    {
        const auto x = safe_num::u8{200U};
        const auto y = safe_num::u32{100U};
        // Convert x to u32 explicitly, then add
        const auto result = safe_num::u32{static_cast<std::uint32_t>(x)} + y;
        std::cout << "safe_numbers explicit: u32(u8(200)) + u32(100) = "
                  << result << std::endl;
        // Output: 300
    }

    // -----------------------------------------------------------------------
    // 6. Mixed safe/built-in arithmetic
    // -----------------------------------------------------------------------
    std::cout << "\n--- Mixed safe/built-in arithmetic ---\n";

    // SafeNumerics: safe<T> can operate directly with built-in types
    {
        safe_nrc::safe<std::uint8_t> x = 200;
        auto result = x + 100;  // int operand is implicitly accepted
        std::cout << "safe_numerics: safe<u8>(200) + 100 = "
                  << static_cast<int>(result) << std::endl;
        // Output: 300
    }

    // SafeNumbers: mixing safe types with built-in types is a compile-time error
    //   safe_num::u8{200U} + 100;          // error: no matching operator+
    //   safe_num::u8{200U} + std::uint8_t{100};  // error: no matching operator+

    // -----------------------------------------------------------------------
    // 7. Unary minus on unsigned types
    // -----------------------------------------------------------------------
    // SafeNumbers: unary minus is a compile-time error via static_assert
    //   auto neg = -safe_num::u8{5U};  // static_assert: "Unary minus is deliberately
    //                             //  disabled for unsigned safe integers"

    // SafeNumerics: unary minus promotes uint8_t to int, so -5 is valid
    std::cout << "\n--- Unary minus on unsigned ---\n";
    try
    {
        safe_nrc::safe<std::uint8_t> x = 5;
        auto result = -x;  // promoted to safe<int>, value is -5
        std::cout << "safe_numerics: -5u = " << static_cast<int>(result) << std::endl;
        // Output: -5 (promoted to int, no error!)
    }
    catch (const std::exception& e)
    {
        std::cout << "safe_numerics threw on unary minus: " << e.what() << std::endl;
    }

    // -----------------------------------------------------------------------
    // 8. Narrowing conversions
    // -----------------------------------------------------------------------
    // Both libraries catch narrowing - this is where SafeNumerics' promotion
    // would eventually trigger an error (when assigning the promoted result
    // back to a narrow type).
    std::cout << "\n--- Narrowing conversions ---\n";

    // SafeNumbers: explicit cast throws std::domain_error if value doesn't fit
    try
    {
        const auto wide = safe_num::u32{300U};
        const auto narrow = static_cast<std::uint8_t>(wide);
        std::cout << "safe_numbers: u32(300) -> u8 = "
                  << static_cast<int>(narrow) << std::endl;
    }
    catch (const std::domain_error& e)
    {
        std::cout << "safe_numbers narrowing threw: " << e.what() << std::endl;
    }

    // SafeNumerics: implicit narrowing throws
    try
    {
        safe_nrc::safe<std::uint32_t> wide = 300;
        safe_nrc::safe<std::uint8_t> narrow = wide;
        std::cout << "safe_numerics implicit: u32(300) -> u8 = "
                  << static_cast<int>(narrow) << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "safe_numerics implicit narrowing threw: " << e.what() << std::endl;
    }

    // SafeNumerics: explicit narrowing via static_cast also throws
    try
    {
        safe_nrc::safe<std::uint32_t> wide = 300;
        auto narrow = static_cast<safe_nrc::safe<std::uint8_t>>(wide);
        std::cout << "safe_numerics explicit: u32(300) -> u8 = "
                  << static_cast<int>(narrow) << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "safe_numerics explicit narrowing threw: " << e.what() << std::endl;
    }

    return 0;
}

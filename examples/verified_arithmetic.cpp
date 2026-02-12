// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Verified types enforce that all arithmetic is evaluated at compile time.
// If an operation would overflow, it produces a compile error instead of
// a runtime exception.

#include <boost/safe_numbers/verified_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>

int main()
{
    using namespace boost::safe_numbers;

    // All arithmetic is consteval - evaluated entirely by the compiler.
    // The results are compile-time constants that cannot be modified at runtime.

    // Basic arithmetic
    constexpr auto sum  {verified_u32{100U} + verified_u32{200U}};
    constexpr auto diff {verified_u32{200U} - verified_u32{100U}};
    constexpr auto prod {verified_u32{15U}  * verified_u32{20U}};
    constexpr auto quot {verified_u32{300U} / verified_u32{10U}};
    constexpr auto rem  {verified_u32{17U}  % verified_u32{5U}};

    std::cout << "100 + 200 = " << sum  << '\n';
    std::cout << "200 - 100 = " << diff << '\n';
    std::cout << "15  * 20  = " << prod << '\n';
    std::cout << "300 / 10  = " << quot << '\n';
    std::cout << "17  % 5   = " << rem  << '\n';

    std::cout << '\n';

    // Compound assignment and increment/decrement are also consteval.
    // They work inside consteval contexts like consteval functions:
    constexpr auto incremented = []() consteval {
        auto val = verified_u8{u8{10}};
        ++val;
        return val;
    }();
    std::cout << "10 after ++: " << incremented << '\n';

    constexpr auto accumulated = []() consteval {
        auto val = verified_u16{u16{0}};
        val += verified_u16{u16{100}};
        val += verified_u16{u16{200}};
        val += verified_u16{u16{300}};
        return val;
    }();
    std::cout << "0 + 100 + 200 + 300 = " << accumulated << '\n';

    std::cout << '\n';

    // Overflow is caught at compile time.
    // These would produce compile errors:
    //
    // constexpr auto overflow {verified_u8{u8{255}} + verified_u8{u8{1}}};
    //   error: consteval call to overflow-checking addition fails
    //
    // constexpr auto underflow {verified_u8{u8{0}} - verified_u8{u8{1}}};
    //   error: consteval call to overflow-checking subtraction fails

    // Bounded arithmetic is also compile-time checked
    constexpr auto bounded_sum {
        verified_bounded_integer<0u, 1000u>{500u} +
        verified_bounded_integer<0u, 1000u>{400u}
    };
    std::cout << "bounded 500 + 400 = " << bounded_sum << '\n';

    return 0;
}

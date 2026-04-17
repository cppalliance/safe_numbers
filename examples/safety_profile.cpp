// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// This example demonstrates how Boost.SafeNumbers can emulate an integer
// safety profile, analogous to the memory safety profiles being proposed
// for C++.
// Under such a profile, a conforming implementation could
// transparently replace the fundamental integer types with checked
// equivalents, so that overflow and underflow become diagnosable errors
// instead of silent wrap-around (which can mask real bugs) or undefined
// behavior (for signed types).
//
// The canonical substitution would look like:
//
// #define std::uint32_t boost::safe_numbers::u32
//
// This is not legal C++ since macro names cannot be qualified identifiers,
// but it illustrates what a compiler driving the profile could do
// automatically when BOOST_SAFE_NUMBERS_INTEGER_SAFETY_PROFILE is set.
// In the meantime, the same effect can be opted into locally with a type
// alias, which is what we do below so the example compiles either way.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <exception>
#include <iostream>
#include <cstdint>
#include <limits>

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4127) // Conditional expression is constant
#endif

#ifdef BOOST_SAFE_NUMBERS_INTEGER_SAFETY_PROFILE
using u32 = boost::safe_numbers::u32;
#else
using u32 = std::uint32_t;
#endif

int main()
{
    try
    {
        // A classic allocation-size bug: the caller supplies an element
        // count and element size, and we compute the total number of
        // bytes to allocate. For large inputs the multiplication
        // overflows a 32-bit unsigned value and wraps to a small number,
        // which would then silently underallocate.
        // The subsequent writes then overflow the buffer.
        const u32 element_count {std::numeric_limits<std::uint32_t>::max() / 2U + 10U};
        const u32 element_size  {4U};
        const u32 total_bytes   {element_count * element_size};

        std::cout << "element_count = " << element_count << '\n';
        std::cout << "element_size  = " << element_size  << '\n';
        std::cout << "total_bytes   = " << total_bytes   << '\n';

        if (total_bytes < element_count)
        {
            std::cout << "Manual post-hoc overflow check fired\n";
        }
        else
        {
            std::cout << "No overflow detected -- but the value is wrong\n";
        }
    }
    catch (const std::exception& e)
    {
        // With BOOST_SAFE_NUMBERS_INTEGER_SAFETY_PROFILE defined, the
        // multiplication above throws before total_bytes can be used,
        // so the bug is surfaced at its source rather than at the
        // eventual out-of-bounds write.
        std::cerr << "Safety profile caught overflow: " << e.what() << '\n';
    }

    return 0;
}

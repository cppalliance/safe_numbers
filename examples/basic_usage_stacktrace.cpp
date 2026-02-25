// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/unsigned_integers.hpp>     // For safe_numbers types
#include <boost/safe_numbers/iostream.hpp>              // For safe_numbers <iostream> support
#include <boost/exception/diagnostic_information.hpp>   // For boost::diagnostic_information

#if (defined(__x86_64__) || defined(_M_X64)) && !defined(__MINGW32__)

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#include <boost/stacktrace.hpp>                         // For boost::stacktrace::from_current_exception

#endif

#include <iostream>
#include <cstdint>

int main()
{
    using boost::safe_numbers::u8;

    try
    {
        const u8 x {UINT8_MAX};
        const u8 y {2};
        const u8 z {x + y};

        std::cout << "Value of z: " << z << std::endl;
    }
    catch (const std::exception& e)
    {
        // BOOST_THROW_EXCEPTION captures the source location (file, line, function)
        // at the throw site. boost::diagnostic_information extracts all of it.
        std::cerr << boost::diagnostic_information(e) << std::endl;

        #if (defined(__x86_64__) || defined(_M_X64)) && !defined(__MINGW32__)
        // On x86_64, boost::stacktrace can capture the throw-site stacktrace
        // from the current exception via from_current_exception().
        // This requires linking with boost_stacktrace_from_exception.
        const auto st {boost::stacktrace::stacktrace::from_current_exception()};
        if (!st.empty())
        {
            std::cerr << "Stacktrace:\n" << st << std::endl;
        }
        #endif
    }

    return 0;
}

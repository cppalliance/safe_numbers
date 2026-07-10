//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Guards the std-exception-type to error_category mapping used by the CUDA
// device error reporter. In particular std::invalid_argument must map to its
// own category rather than being folded into domain_error, so that a device
// (or CUDA host) invalid_argument is rethrown as std::invalid_argument.

#include <boost/safe_numbers/device_error_reporting.hpp>
#include <stdexcept>

using boost::safe_numbers::detail::exception_type;
using boost::safe_numbers::detail::to_exception_enum;

static_assert(to_exception_enum<std::domain_error>()     == exception_type::domain_error);
static_assert(to_exception_enum<std::overflow_error>()   == exception_type::overflow);
static_assert(to_exception_enum<std::underflow_error>()  == exception_type::underflow);
static_assert(to_exception_enum<std::invalid_argument>() == exception_type::invalid_argument);
static_assert(to_exception_enum<std::runtime_error>()    == exception_type::unknown);

int main()
{
    return 0;
}

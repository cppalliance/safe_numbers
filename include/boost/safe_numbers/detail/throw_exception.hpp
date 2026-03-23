// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// BOOST_THROW_EXCEPTION fits our needs for CPU tasks
// We need to define our own version of thrown exception so that we can also handle the device environment
// This is the only file in the lib that should attempt inclusion of boost/throw_exception.hpp

#ifndef BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP
#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP

#include <boost/safe_numbers/detail/cuda_error_reporting.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <stdexcept>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// Two-argument form: (exception_type, message)
// On host: constructs and throws the exception with the message
// On CUDA device: passes the const char* message to the device error reporter
#ifndef __CUDACC__

#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION(exc_type, msg) BOOST_THROW_EXCEPTION(exc_type(msg))

#else

#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION(exc_type, msg) boost::safe_numbers::detail::report_device_error(__FILE__, __LINE__, msg)

#endif // __CUDACC__

#endif // BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// BOOST_THROW_EXCEPTION fits our needs for CPU tasks
// We need to define our own version of thrown exception so that we can also handle the device environment
// This is the only file in the lib that should attempt inclusion of boost/throw_exception.hpp

#ifndef BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP
#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP

#include <boost/safe_numbers/device_error_reporting.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// Two-argument form: (exception_type, message)
// On host: constructs and throws the exception with the message.
// On a GPU device backend (CUDA or SYCL): forwards to the device error reporter,
// which itself branches host/device internally.
#ifdef BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT

#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION(exc_type, msg) boost::safe_numbers::detail::report_device_error(boost::safe_numbers::detail::to_exception_enum<exc_type>(), __FILE__, __LINE__, msg)

#else

#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION(exc_type, msg) BOOST_THROW_EXCEPTION(exc_type(msg))

#endif // BOOST_SAFE_NUMBERS_HAS_GPU_SUPPORT

#endif // BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP

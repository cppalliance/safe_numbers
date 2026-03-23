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

// This is defined only in the device compilation step
// as opposed to __NVCC__
#ifndef __CUDACC__

#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION(x) BOOST_THROW_EXCEPTION(x)

#else

#define BOOST_SAFE_NUMBERS_THROW_EXCEPTION(x) boost::safe_numbers::detail::report_device_error(__FILE__, __LINE__, #x)

#endif // __CUDA_ARCH__

#endif // BOOST_SAFE_NUMBERS_THROW_EXCEPTION_HPP

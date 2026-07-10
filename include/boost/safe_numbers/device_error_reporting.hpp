// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Public entry point for device error reporting. Include this to obtain
// device_error_context. It pulls in the backend-neutral pieces (so
// device_exception_mode is always available) and dispatches to the CUDA or SYCL
// backend based on compiler detection and the enable macro.
//
//   * CUDA: nvcc (__CUDACC__) + BOOST_SAFE_NUMBERS_ENABLE_CUDA
//   * SYCL: BOOST_SAFE_NUMBERS_ENABLE_SYCL (with <sycl/sycl.hpp> included first)
//
// On a pure host build neither backend is included and only device_exception_mode
// / the detail enums are defined.

#ifndef BOOST_SAFE_NUMBERS_DEVICE_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_DEVICE_ERROR_REPORTING_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/device_error_common.hpp>

#if defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA)
#  include <boost/safe_numbers/detail/cuda_error_reporting.hpp>
#elif defined(BOOST_SAFE_NUMBERS_ENABLE_SYCL)
#  include <boost/safe_numbers/detail/sycl_error_reporting.hpp>
#endif

#endif // BOOST_SAFE_NUMBERS_DEVICE_ERROR_REPORTING_HPP

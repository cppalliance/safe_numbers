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

#if (defined(__CUDACC__) && defined(BOOST_SAFE_NUMBERS_ENABLE_CUDA)) || defined(BOOST_SAFE_NUMBERS_ENABLE_SYCL)

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE
#include <utility>
#endif

namespace boost::safe_numbers {

// Deferred device error reporting for kernels built on the overflowing_*
// families. The per-operation flags accumulate branch-free, so the kernel
// body stays vectorizable on CPU style devices where the per-operation report
// branch of the throwing operators inhibits SPMD vectorization. report() is
// the one branch per work item: on the device it records into the same error
// state that device_error_context::synchronize() rethrows from, and on the
// host it throws immediately. The individual error categories are not
// preserved; a deferred error always surfaces as overflow.
class deferred_errors
{
public:
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr void accumulate(const bool had_error) noexcept
    {
        flags_ |= static_cast<unsigned>(had_error);
    }

    // Accumulates the flag of an overflowing_* result and returns its value
    template <typename T>
    BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto unwrap(const std::pair<T, bool> result) noexcept -> T
    {
        accumulate(result.second);
        return result.first;
    }

    BOOST_SAFE_NUMBERS_HOST_DEVICE [[nodiscard]] constexpr auto any() const noexcept -> bool
    {
        return flags_ != 0U;
    }

    // Call once at the end of the kernel body
    BOOST_SAFE_NUMBERS_HOST_DEVICE void report(const char* file = "deferred_errors",
                                               const int line = 0,
                                               const char* expression = "deferred checked arithmetic") const
    {
        if (flags_ != 0U)
        {
            detail::report_device_error(detail::exception_type::overflow, file, line, expression);
        }
    }

private:
    unsigned flags_ {0U};
};

} // namespace boost::safe_numbers

#endif

#endif // BOOST_SAFE_NUMBERS_DEVICE_ERROR_REPORTING_HPP

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// CUDA device error-reporting backend. Selected by the public
// device_error_reporting.hpp dispatcher when compiling with nvcc and
// BOOST_SAFE_NUMBERS_ENABLE_CUDA. Backend-neutral pieces live in
// detail/device_error_common.hpp.

#ifndef BOOST_SAFE_NUMBERS_DETAIL_CUDA_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_CUDA_ERROR_REPORTING_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/device_error_common.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <string>
#include <stdexcept>
#include <sstream>

#ifdef __CUDACC__
#include <cuda_runtime.h>
#endif

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

namespace detail {

#ifdef __CUDACC__

// Managed memory error record accessible from both host and device.
// Since we never destroy the CUDA context, __managed__ is safe to use.
__managed__ device_error_record g_device_error {};

// Managed enum controlling what report_device_error does on failure.
// Default to trapped as that's the best way to ensure hard failure on error.
__managed__ device_exception_mode g_device_fail_type {device_exception_mode::trapped};

__host__ __device__ inline void report_device_error(
    exception_type exc,
    const char* file,
    int line,
    const char* expression)
{
    #ifdef __CUDA_ARCH__

    if (atomicCAS(&g_device_error.flag, 0, 1) == 0)
    {
        g_device_error.line = line;
        g_device_error.thread_id = blockIdx.x * blockDim.x + threadIdx.x;
        g_device_error.exception = exc;

        copy_to_buf(g_device_error.file, file, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
        copy_to_buf(g_device_error.expression, expression, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
        __threadfence_system();

        if (g_device_fail_type == device_exception_mode::trapped)
        {
            __trap();
        }
    }

    switch (g_device_fail_type)
    {
        case device_exception_mode::trapped:
            // In the event that __trap() is called the error is non-recoverable.
            // Other threads: spin until the trap terminates the kernel.
            while (true)
            {
                __nanosleep(1000000);
            }
            break;

        case device_exception_mode::untrapped:
            // Return instead of trapping. The kernel completes; synchronize()
            // detects the error via the flag and throws on the host.
            return;
            break;
    }

    #else

    const auto msg = std::string(file) + ":" + std::to_string(line) + ": " + expression;
    throw_from_exception_type(exc, msg);

    #endif
}

#endif // __CUDACC__

} // namespace detail

#ifdef __CUDACC__

class device_error_context
{
public:

    // Clears the error state. Only one device_error_context may exist at a time.
    device_error_context()
    {
        if (detail::g_device_error_context_active)
        {
            BOOST_THROW_EXCEPTION(std::logic_error(
                "Only one device_error_context may exist at a time"));
        }
        detail::g_device_error_context_active = true;
        reset();
    }

    // Sets the failure mode on the managed global up front.
    device_error_context(const device_exception_mode e)
    {
        if (detail::g_device_error_context_active)
        {
            BOOST_THROW_EXCEPTION(std::logic_error(
                "Only one device_error_context may exist at a time"));
        }

        detail::g_device_fail_type = e;

        detail::g_device_error_context_active = true;
        reset();
    }

    ~device_error_context()
    {
        detail::g_device_error_context_active = false;
    }

    device_error_context(const device_error_context&) = delete;
    device_error_context& operator=(const device_error_context&) = delete;

    // Clears the error fields so the context can be reused across kernel launches.
    void reset()
    {
        detail::g_device_error.flag = 0;
        detail::g_device_error.line = 0;
        detail::g_device_error.thread_id = 0;
        detail::g_device_error.exception = detail::exception_type::unknown;
        detail::g_device_error.file[0] = '\0';
        detail::g_device_error.expression[0] = '\0';
    }

    // Post-construction way of setting the failure mode for the device.
    void set_device_exception_method(const device_exception_mode e)
    {
        detail::g_device_fail_type = e;
    }

    // Synchronizes the device and checks for errors captured by device code.
    // If an error was detected, the error state is cleared (so the context is
    // immediately reusable), and the appropriate std::exception is thrown.
    void synchronize()
    {
        const auto status = cudaDeviceSynchronize();

        const auto flag = detail::g_device_error.flag;

        if (flag != 0)
        {
            const auto thread_id = detail::g_device_error.thread_id;
            const auto line = detail::g_device_error.line;
            const auto exc = detail::g_device_error.exception;

            std::ostringstream oss;
            oss << "Device error on thread " << thread_id
                << " at " << detail::g_device_error.file
                << ":" << line
                << ": " << detail::g_device_error.expression;

            const auto msg = oss.str();

            // Clear the error state so the context can be reused
            // immediately after catching the exception.
            reset();

            detail::throw_from_exception_type(exc, msg);
        }
        else
        {
            reset();
        }

        if (status != cudaSuccess)
        {
            cudaGetLastError();
            BOOST_THROW_EXCEPTION(std::runtime_error(cudaGetErrorString(status)));
        }
    }
};

#endif // __CUDACC__

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_DETAIL_CUDA_ERROR_REPORTING_HPP

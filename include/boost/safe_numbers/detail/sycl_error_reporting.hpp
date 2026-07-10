// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// SYCL device error-reporting backend. Selected by the public
// device_error_reporting.hpp dispatcher when BOOST_SAFE_NUMBERS_ENABLE_SYCL is
// set. Requires <sycl/sycl.hpp> to have been included first. Backend-neutral
// pieces live in detail/device_error_common.hpp.

#ifndef BOOST_SAFE_NUMBERS_DETAIL_SYCL_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_SYCL_ERROR_REPORTING_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/device_error_common.hpp>

#if defined(BOOST_SAFE_NUMBERS_ENABLE_SYCL)

#ifndef SYCL_EXTERNAL
#  error "Include <sycl/sycl.hpp> before <boost/safe_numbers/...> when BOOST_SAFE_NUMBERS_ENABLE_SYCL is defined"
#endif

// The device error buffer (g_device_error below) is a device_global, which core
// SYCL 2020 does not provide - section 5.4 forbids mutable variables with static
// storage duration in device code. device_global is the sycl_ext_oneapi_device_global
// extension that lifts that restriction, and it is the only way a deeply-nested
// operation can reach the shared error record without threading a pointer through
// every call. Require the extension explicitly (its feature-test macro) rather than
// letting device_global fail to resolve with a cryptic diagnostic on implementations
// that do not support it.
#ifndef SYCL_EXT_ONEAPI_DEVICE_GLOBAL
#  error "Boost.safe_numbers SYCL device error reporting requires the device_global extension (feature-test macro SYCL_EXT_ONEAPI_DEVICE_GLOBAL), which this SYCL implementation does not advertise"
#endif

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <string>
#include <stdexcept>
#include <sstream>
#include <utility>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

namespace detail {

// SYCL analog of the CUDA __managed__ error buffer. A single inline instance
// (the SYCL 2020 device_global extension explicitly permits inline) that any
// device-side function can write by name, giving the same zero-pointer-plumbing
// property as __managed__. No device_image_scope property is set, so host
// queue.copy access is unrestricted (the variable is backed by a shared buffer).
inline sycl::ext::oneapi::experimental::device_global<device_error_record> g_device_error;

// Reports an error from device or host. Compiled for both passes of the
// single-source TU: the device pass records into the device_global (first writer
// wins, then returns, since SYCL has no recoverable device trap); the host pass
// throws the matching std exception. Mirrors how the CUDA reporter branches on
// __CUDA_ARCH__.
BOOST_SAFE_NUMBERS_HOST_DEVICE inline void report_device_error(
    exception_type exc,
    const char* file,
    int line,
    const char* expression)
{
    #ifdef __SYCL_DEVICE_ONLY__

    auto& rec = g_device_error.get();
    sycl::atomic_ref<int,
        sycl::memory_order::relaxed,
        sycl::memory_scope::device,
        sycl::access::address_space::global_space> flag_ref(rec.flag);

    // First failing work-item wins and records the details; losers never touch
    // the payload, so the host can never observe a torn record.
    int expected {0};
    if (flag_ref.compare_exchange_strong(expected, 1))
    {
        rec.line = line;
        rec.exception = exc;
        #if defined(SYCL_EXT_ONEAPI_FREE_FUNCTION_QUERIES) && !defined(BOOST_SAFE_NUMBERS_SYCL_DISABLE_WORKITEM_ID)
        // The failing work-item's dimension-0 global linear id, the analog of
        // the CUDA backend's blockIdx.x * blockDim.x + threadIdx.x. Core SYCL
        // 2020 only passes the id into the kernel lambda, so reaching it from
        // here needs the free-function queries extension; on DPC++ it returns
        // the correct id for plain range kernels, nd_range kernels, and 0 for
        // single_task. Kernels launched over more than one dimension report
        // only dimension 0 (same limitation as the CUDA backend).
        rec.thread_id = static_cast<int>(
            sycl::ext::oneapi::this_work_item::get_nd_item<1>().get_global_linear_id());
        #else
        rec.thread_id = -1;
        #endif
        copy_to_buf(rec.file, file, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
        copy_to_buf(rec.expression, expression, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
    }

    // No __trap()/recoverable device abort in SYCL: record and return.
    // synchronize() surfaces the error on the host after the kernel completes.
    return;

    #else

    const auto msg = std::string(file) + ":" + std::to_string(line) + ": " + expression;
    throw_from_exception_type(exc, msg);

    #endif
}

} // namespace detail

// Host-facing RAII context placed around SYCL kernel launches. Mirrors the CUDA
// device_error_context but binds to the user's queue (SYCL has no implicit
// device). Only one may exist at a time. Defined on both compiler passes (its
// methods are host-only and never called from device code, so the device pass
// parses but does not codegen them) so that host code referencing it parses on
// the device pass too.
class device_error_context
{
    sycl::queue queue_;
    [[maybe_unused]] device_exception_mode mode_ {device_exception_mode::untrapped};

    static void guard()
    {
        if (detail::g_device_error_context_active)
        {
            BOOST_THROW_EXCEPTION(std::logic_error(
                "Only one device_error_context may exist at a time"));
        }
    }

public:

    explicit device_error_context(sycl::queue q)
        : queue_ {std::move(q)}
    {
        guard();
        detail::g_device_error_context_active = true;
        reset();
    }

    // The mode is advisory under SYCL (always deferred); kept for symmetry.
    device_error_context(sycl::queue q, const device_exception_mode e)
        : queue_ {std::move(q)}, mode_ {e}
    {
        guard();
        detail::g_device_error_context_active = true;
        reset();
    }

    ~device_error_context()
    {
        detail::g_device_error_context_active = false;
    }

    device_error_context(const device_error_context&) = delete;
    device_error_context& operator=(const device_error_context&) = delete;

    // Zero the device buffer (host -> device) so the context is reusable.
    void reset()
    {
        const detail::device_error_record zero {};
        queue_.copy(&zero, detail::g_device_error).wait();
    }

    // SYCL is always deferred; retained for source-level symmetry with CUDA.
    void set_device_exception_method(const device_exception_mode e)
    {
        mode_ = e;
    }

    // Wait for all enqueued kernels, read the buffer back (device -> host), and
    // if an error was captured clear it and throw the matching std::exception.
    void synchronize()
    {
        queue_.wait();

        detail::device_error_record rec {};
        queue_.copy(detail::g_device_error, &rec).wait();

        if (rec.flag != 0)
        {
            std::ostringstream oss;
            oss << "Device error on thread " << rec.thread_id
                << " at " << rec.file
                << ":" << rec.line
                << ": " << rec.expression;

            const auto msg = oss.str();

            // Clear before throwing so the context is reusable.
            reset();

            detail::throw_from_exception_type(rec.exception, msg);
        }
        else
        {
            reset();
        }
    }
};

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_ENABLE_SYCL

#endif // BOOST_SAFE_NUMBERS_DETAIL_SYCL_ERROR_REPORTING_HPP

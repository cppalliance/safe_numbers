// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Backend-neutral pieces shared by the CUDA and SYCL device error reporters.
// Included by detail/cuda_error_reporting.hpp, detail/sycl_error_reporting.hpp,
// and the public device_error_reporting.hpp dispatcher.

#ifndef BOOST_SAFE_NUMBERS_DETAIL_DEVICE_ERROR_COMMON_HPP
#define BOOST_SAFE_NUMBERS_DETAIL_DEVICE_ERROR_COMMON_HPP

#include <boost/safe_numbers/detail/config.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <string>
#include <stdexcept>
#include <type_traits>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// Using a macro instead of a global constant because inline constexpr is not available on device
#ifdef PATH_MAX
#  define BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE PATH_MAX
#else
#  define BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE 512
#endif

namespace boost::safe_numbers {

// Selects how the device reacts to a captured error. On CUDA, trapped hard-fails
// the kernel via __trap() while untrapped defers to the host. SYCL has no
// recoverable device trap, so under SYCL it is always deferred (trapped degrades
// to untrapped); the enum is retained for source-level symmetry with CUDA.
enum class device_exception_mode : unsigned
{
    trapped,
    untrapped,
};

inline constexpr auto trapped = device_exception_mode::trapped;
inline constexpr auto untrapped = device_exception_mode::untrapped;

namespace detail {

enum class exception_type : unsigned
{
    domain_error,
    overflow,
    underflow,
    invalid_argument,
    unknown,
};

// POD error record copied by value between device and host.
struct device_error_record
{
    int  flag;                                                      // 0 = no error, 1 = error captured
    int  line;                                                      // __LINE__
    int  thread_id;                                                 // thread / work-item id (-1 = unknown)
    exception_type exception;                                       // exception that would have been thrown on CPU
    char file[BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE];         // __FILE__ copied by value
    char expression[BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE];   // message copied by value
};

// Back-compat alias for the original CUDA-specific name.
using cuda_device_error = device_error_record;

// Compile-time map from a std exception type to our enum.
template <typename T>
BOOST_SAFE_NUMBERS_HOST_DEVICE constexpr auto to_exception_enum() noexcept -> exception_type
{
    if constexpr (std::is_same_v<T, std::domain_error>)
    {
        return exception_type::domain_error;
    }
    else if constexpr (std::is_same_v<T, std::overflow_error>)
    {
        return exception_type::overflow;
    }
    else if constexpr (std::is_same_v<T, std::underflow_error>)
    {
        return exception_type::underflow;
    }
    else if constexpr (std::is_same_v<T, std::invalid_argument>)
    {
        return exception_type::invalid_argument;
    }
    else
    {
        return exception_type::unknown;
    }
}

// Device-safe bounded string copy (device has no strncpy).
BOOST_SAFE_NUMBERS_HOST_DEVICE inline void copy_to_buf(char* dst, const char* src, const int max_len)
{
    int i = 0;
    for (; i < max_len - 1 && src[i] != '\0'; ++i)
    {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

// Host-side translation from the enum to a real std exception. Used by both
// backends' host branch and by both device_error_context::synchronize() methods.
inline void throw_from_exception_type(const exception_type exc, const std::string& msg)
{
    switch (exc)
    {
        case exception_type::domain_error:
            BOOST_THROW_EXCEPTION(std::domain_error(msg));
            break;
        case exception_type::overflow:
            BOOST_THROW_EXCEPTION(std::overflow_error(msg));
            break;
        case exception_type::underflow:
            BOOST_THROW_EXCEPTION(std::underflow_error(msg));
            break;
        case exception_type::invalid_argument:
            BOOST_THROW_EXCEPTION(std::invalid_argument(msg));
            break;
        case exception_type::unknown:
            [[fallthrough]];
        default:
            BOOST_THROW_EXCEPTION(std::runtime_error(msg));
    }
}

// Tracks whether a device_error_context instance is alive. Only one may exist at
// a time to prevent races on the device error buffer.
inline bool g_device_error_context_active = false;

} // namespace detail
} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_DETAIL_DEVICE_ERROR_COMMON_HPP

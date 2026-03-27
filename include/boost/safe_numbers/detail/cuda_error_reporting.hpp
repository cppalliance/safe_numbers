// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP

#include <boost/safe_numbers/detail/config.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <string>
#include <stdexcept>
#include <sstream>

#ifdef __CUDACC__
#include <cuda_runtime.h>
#endif

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

// Using a macro instead of a global constant because the inline constexpr is not available on device
#ifdef PATH_MAX
#  define BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE PATH_MAX
#else
#  define BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE 512
#endif

namespace boost::safe_numbers {

namespace detail {

enum class exception_type : unsigned
{
    domain_error,
    overflow,
    underflow,
    unknown,
};

struct cuda_device_error
{
    int  flag;                                                      // 0 = no error, 1 = error captured
    int  line;                                                      // __LINE__
    int  thread_id;                                                 // blockIdx.x * blockDim.x + threadIdx.x
    exception_type exception;                                       // Type of exception that would have been thrown on CPU
    char file[BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE];         // __FILE__ copied by value
    char expression[BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE];   // x copied by value
};

// Compile-time map from std exception type to our enum
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
        return exception_type::domain_error;
    }
    else
    {
        return exception_type::unknown;
    }
}

BOOST_SAFE_NUMBERS_HOST_DEVICE inline void copy_to_buf(char* dst, const char* src, const int max_len)
{
    int i = 0;
    for (; i < max_len - 1 && src[i] != '\0'; ++i)
    {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

#ifdef __CUDACC__

// Managed memory error struct accessible from both host and device.
// Since we never destroy the CUDA context, __managed__ is safe to use.
__managed__ cuda_device_error g_device_error {};

// Tracks whether a device_error_context instance is alive.
// Only one may exist at a time to prevent races on g_device_error.
inline bool g_device_error_context_active = false;

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
    }

    // Return instead of calling __trap(). This allows the kernel to
    // complete normally without corrupting the CUDA context. Other
    // threads may continue with incorrect values, but synchronize()
    // will detect the error via the flag and throw on the host.
    return;
    #else

    const auto msg = std::string(file) + ":" + std::to_string(line) + ": " + expression;
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
        case exception_type::unknown:
            [[fallthrough]];
        default:
            BOOST_THROW_EXCEPTION(std::runtime_error(msg));
    }

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

    // Synchronizes the device and checks for errors captured by device code.
    // If an error was detected, the error state is cleared (so the context
    // is immediately reusable), and the appropriate std::exception is thrown.
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

            switch (exc)
            {
                case detail::exception_type::domain_error:
                    BOOST_THROW_EXCEPTION(std::domain_error(msg));
                    break;
                case detail::exception_type::overflow:
                    BOOST_THROW_EXCEPTION(std::overflow_error(msg));
                    break;
                case detail::exception_type::underflow:
                    BOOST_THROW_EXCEPTION(std::underflow_error(msg));
                    break;
                case detail::exception_type::unknown:
                    [[fallthrough]];
                default:
                    BOOST_THROW_EXCEPTION(std::runtime_error(msg));
            }
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

#endif // BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP

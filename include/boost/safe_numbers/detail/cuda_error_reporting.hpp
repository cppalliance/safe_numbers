// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP

#include <boost/safe_numbers/detail/config.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

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

// __managed__ places this in unified memory so the host can read it directly
// without cudaMemcpyFromSymbol, which fails after __trap() corrupts the device context
__managed__ cuda_device_error g_device_error = {0, 0, 0, exception_type::unknown, {'\0'}, {'\0'}};

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

        printf("Device error at: [GPU thread %d] %s:%d: %s\n",
               blockIdx.x * blockDim.x + threadIdx.x,
               file, line, expression);

        __trap();
    }

    // Other threads: spin until the trap terminates the kernel
    while (true)
    {
        __nanosleep(1000000);
    }
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

    // Clears the global state
    // The error context can be reused with multiple kernels if this is called
    void reset()
    {
        detail::g_device_error.flag = 0;
        detail::g_device_error.line = 0;
        detail::g_device_error.thread_id = 0;
        detail::g_device_error.exception = detail::exception_type::unknown;
        detail::g_device_error.file[0] = '\0';
        detail::g_device_error.expression[0] = '\0';
    }

    // On construction, reset the global error state to ensure we have a good start
    device_error_context()
    {
        reset();
    }

    // Allows the user to synchronize and check for errors as is typical of CUDA
    // This allows an extra step in that it will throw on the host
    // Much like cudaGetLastError, the call to synchronize will destroy the information in the global context
    // This allows trivial reuse of all these facilities
    void synchronize()
    {
        const auto status = cudaDeviceSynchronize();

        // Read directly from managed memory — no cudaMemcpyFromSymbol needed
        // This works even after __trap() corrupts the device context
        const auto flag = detail::g_device_error.flag;
        const auto thread_id = detail::g_device_error.thread_id;
        const auto line = detail::g_device_error.line;

        if (flag != 0)
        {
            std::ostringstream oss;
            oss << "Device error on thread " << thread_id
                << " at " << detail::g_device_error.file
                << ":" << line
                << ": " << detail::g_device_error.expression;

            // Read exception type before reset clears it
            const auto exc = detail::g_device_error.exception;

            // Clear the sticky CUDA error and reset our state
            cudaGetLastError();
            reset();

            switch (exc)
            {
                case detail::exception_type::domain_error:
                    BOOST_THROW_EXCEPTION(std::domain_error(oss.str()));
                    break;
                case detail::exception_type::overflow:
                    BOOST_THROW_EXCEPTION(std::overflow_error(oss.str()));
                    break;
                case detail::exception_type::underflow:
                    BOOST_THROW_EXCEPTION(std::underflow_error(oss.str()));
                    break;
                case detail::exception_type::unknown:
                    [[fallthrough]];
                default:
                    BOOST_THROW_EXCEPTION(std::runtime_error(oss.str()));
            }
        }

        reset();

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

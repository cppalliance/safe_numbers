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

// __managed__ pointer to dynamically allocated managed memory.
// Using a pointer (rather than a __managed__ struct) lets us free and
// re-allocate after cudaDeviceReset(), which is required to recover
// from __trap() corrupting the device context.
// The pointer itself lives in managed memory so device code can
// dereference it directly.
__managed__ cuda_device_error* g_device_error = nullptr;

__host__ __device__ inline void report_device_error(
    exception_type exc,
    const char* file,
    int line,
    const char* expression)
{
    #ifdef __CUDA_ARCH__

    if (g_device_error != nullptr && atomicCAS(&g_device_error->flag, 0, 1) == 0)
    {
        g_device_error->line = line;
        g_device_error->thread_id = blockIdx.x * blockDim.x + threadIdx.x;
        g_device_error->exception = exc;

        copy_to_buf(g_device_error->file, file, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
        copy_to_buf(g_device_error->expression, expression, BOOST_SAFE_NUMBERS_DEVICE_ERROR_BUFFER_SIZE);
        __threadfence_system();

        printf("Device error on thread %d at %s:%d: %s\n",
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

    // Allocates the managed error struct if it does not already exist,
    // then clears the error state. After cudaDeviceReset() the __managed__
    // pointer is back to nullptr, so the next construction re-allocates.
    device_error_context()
    {
        ensure_allocated();
        reset();
    }

    // Free the managed allocation during normal (non-error) shutdown.
    // After cudaDeviceReset(), g_device_error is already nullptr.
    ~device_error_context()
    {
        if (detail::g_device_error != nullptr)
        {
            cudaFree(detail::g_device_error);
            detail::g_device_error = nullptr;
        }
    }

    device_error_context(const device_error_context&) = delete;
    device_error_context& operator=(const device_error_context&) = delete;

    // Clears the error fields so the context can be reused across kernel launches
    void reset()
    {
        if (detail::g_device_error == nullptr)
        {
            ensure_allocated();
        }

        detail::g_device_error->flag = 0;
        detail::g_device_error->line = 0;
        detail::g_device_error->thread_id = 0;
        detail::g_device_error->exception = detail::exception_type::unknown;
        detail::g_device_error->file[0] = '\0';
        detail::g_device_error->expression[0] = '\0';
    }

    // Allows the user to synchronize and check for errors as is typical of CUDA.
    // This allows an extra step in that it will throw on the host.
    //
    // When a device error is detected (flag != 0):
    //   1. The error info is copied to local variables
    //   2. The managed allocation is freed and the device is reset
    //      (required because __trap() corrupts the device context)
    //   3. The appropriate std::exception is thrown
    //
    // After catching the exception, a new device_error_context can be
    // constructed which will re-allocate fresh managed memory.
    void synchronize()
    {
        const auto status = cudaDeviceSynchronize();

        if (detail::g_device_error == nullptr)
        {
            if (status != cudaSuccess)
            {
                cudaGetLastError();
                BOOST_THROW_EXCEPTION(std::runtime_error(cudaGetErrorString(status)));
            }
            return;
        }

        // Read directly from managed memory — no cudaMemcpyFromSymbol needed
        // This works even after __trap() corrupts the device context
        const auto flag = detail::g_device_error->flag;

        if (flag != 0)
        {
            // Copy everything we need to local storage before freeing
            const auto thread_id = detail::g_device_error->thread_id;
            const auto line = detail::g_device_error->line;
            const auto exc = detail::g_device_error->exception;

            std::ostringstream oss;
            oss << "Device error on thread " << thread_id
                << " at " << detail::g_device_error->file
                << ":" << line
                << ": " << detail::g_device_error->expression;

            const auto msg = oss.str();

            // Free the managed allocation and reset the device so that
            // new kernels can be launched after the user catches the exception.
            // cudaDeviceReset() re-initializes the __managed__ pointer to nullptr.
            cudaFree(detail::g_device_error);
            detail::g_device_error = nullptr;
            cudaDeviceReset();

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

        reset();

        if (status != cudaSuccess)
        {
            cudaGetLastError();
            BOOST_THROW_EXCEPTION(std::runtime_error(cudaGetErrorString(status)));
        }
    }

private:

    void ensure_allocated()
    {
        if (detail::g_device_error == nullptr)
        {
            const auto err = cudaMallocManaged(&detail::g_device_error, sizeof(detail::cuda_device_error));
            if (err != cudaSuccess)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error(
                    std::string("Failed to allocate device error context: ") + cudaGetErrorString(err)));
            }
            cudaDeviceSynchronize();
        }
    }
};

#endif // __CUDACC__

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP

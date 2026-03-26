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

// __device__ pointer to dynamically allocated managed memory.
// Using __device__ (not __managed__) means the pointer variable itself
// lives in device memory, not unified memory. This is critical because
// after cudaDeviceReset() a __managed__ variable's backing memory is
// freed and any host-side access segfaults. A __device__ variable is
// re-initialized to its static initializer (nullptr) when the runtime
// restarts, and the host never dereferences it directly — it uses
// cudaMemcpyToSymbol to update it.
__device__ cuda_device_error* g_device_error = nullptr;

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
    // Host-side mirror of the managed allocation pointer.
    // All host-side reads go through this so we never touch the
    // __managed__ variable g_device_error after cudaDeviceReset().
    detail::cuda_device_error* m_allocation {nullptr};

public:

    // Allocates the managed error struct if it does not already exist,
    // then clears the error state. After cudaDeviceReset() the __managed__
    // pointer is back to nullptr, so the next call to reset() or
    // synchronize() re-allocates automatically.
    // Only one device_error_context may exist at a time.
    device_error_context()
    {
        if (detail::g_device_error_context_active)
        {
            BOOST_THROW_EXCEPTION(std::logic_error(
                "Only one device_error_context may exist at a time"));
        }
        detail::g_device_error_context_active = true;
        ensure_allocated();
        reset();
    }

    // Free the managed allocation during normal (non-error) shutdown.
    // Uses the host-side m_allocation pointer so this is safe even
    // after cudaDeviceReset() has invalidated the __managed__ global.
    ~device_error_context()
    {
        if (m_allocation != nullptr)
        {
            cudaFree(m_allocation);
            m_allocation = nullptr;
        }
        detail::g_device_error_context_active = false;
    }

    device_error_context(const device_error_context&) = delete;
    device_error_context& operator=(const device_error_context&) = delete;

    // Clears the error fields so the context can be reused across kernel launches.
    // If the managed buffer was freed (e.g. after a device reset), re-allocates it.
    void reset()
    {
        if (m_allocation == nullptr)
        {
            ensure_allocated();
        }

        m_allocation->flag = 0;
        m_allocation->line = 0;
        m_allocation->thread_id = 0;
        m_allocation->exception = detail::exception_type::unknown;
        m_allocation->file[0] = '\0';
        m_allocation->expression[0] = '\0';
    }

    // Allows the user to synchronize and check for errors as is typical of CUDA.
    // This allows an extra step in that it will throw on the host.
    //
    // When a device error is detected (flag != 0):
    //   1. The error info is copied to local variables
    //   2. The appropriate std::exception is thrown
    //
    // After catching the exception, call reset_after_error() to restore
    // the device and error context before launching new kernels.
    void synchronize()
    {
        const auto status = cudaDeviceSynchronize();

        // Read directly from managed memory via host-side pointer
        // This works even after __trap() corrupts the device context
        const auto flag = m_allocation->flag;

        if (flag != 0)
        {
            // Copy everything we need to local storage before freeing
            const auto thread_id = m_allocation->thread_id;
            const auto line = m_allocation->line;
            const auto exc = m_allocation->exception;

            std::ostringstream oss;
            oss << "Device error on thread " << thread_id
                << " at " << m_allocation->file
                << ":" << line
                << ": " << m_allocation->expression;

            const auto msg = oss.str();

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

    // Call this in a catch block after synchronize() throws a device error.
    // Resets the device (required because __trap() corrupts the device context),
    // clears the sticky CUDA error, re-allocates the managed error buffer,
    // and re-initializes the device global so new kernels can report errors.
    // After this returns, the context is fully ready for new kernel launches.
    void reset_after_error()
    {
        m_allocation = nullptr;

        // The sticky error from __trap() must be drained BEFORE
        // cudaDeviceReset(), otherwise the reset call itself fails
        // silently (all CUDA runtime calls return the sticky error
        // until cudaGetLastError() clears it).
        cudaGetLastError();
        cudaDeviceReset();

        reset();
    }

private:

    void ensure_allocated()
    {
        if (m_allocation == nullptr)
        {
            const auto err = cudaMallocManaged(&m_allocation, sizeof(detail::cuda_device_error));
            if (err != cudaSuccess)
            {
                BOOST_THROW_EXCEPTION(std::runtime_error(
                    std::string("Failed to allocate device error context: ") + cudaGetErrorString(err)));
            }

            // Point the __device__ global at the new allocation so device code can find it.
            // We must use cudaMemcpyToSymbol because g_device_error is in device memory.
            cudaMemcpyToSymbol(detail::g_device_error, &m_allocation, sizeof(detail::cuda_device_error*));
            cudaDeviceSynchronize();
        }
    }
};

#endif // __CUDACC__

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP

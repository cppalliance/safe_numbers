// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP

#include <boost/safe_numbers/detail/config.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <cstdio>
#include <cmath>
#include <string>
#include <stdexcept>
#include <sstream>

#ifdef __CUDACC__
#include <cuda_runtime.h>
#endif

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

namespace detail {

struct cuda_device_error
{
    int         flag;       // 0 = no error, 1 = error captured
    int         line;       // __LINE__
    int         thread_id;  // Approximated by blockIdx.x * blockDim.x + threadIdx.x;
    const char* file;       // __FILE__ string literal
    const char* expression; // #x stringified expression
};

#ifdef __CUDACC__

__device__ cuda_device_error g_device_error = {0, 0, 0, nullptr, nullptr};

__host__ __device__ inline void report_device_error(
    const char* file,
    int line,
    const char* expression)
{
#ifdef __CUDA_ARCH__
    if (atomicCAS(&g_device_error.flag, 0, 1) == 0)
    {
        g_device_error.file       = file;
        g_device_error.line       = line;
        g_device_error.expression = expression;
        g_device_error.thread_id  = blockIdx.x * blockDim.x + threadIdx.x;
        __threadfence_system();
    }

    printf("Device error at: [GPU thread %d] %s:%d: %s\n",
           blockIdx.x * blockDim.x + threadIdx.x,
           file, line, expression);

    __trap();
#else
    BOOST_THROW_EXCEPTION(std::runtime_error(std::string(file) + ":" + std::to_string(line) + ": " + expression));
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
        const detail::cuda_device_error new_error = {0, 0, 0, nullptr, nullptr};
        cudaMemcpyToSymbol(detail::g_device_error, &new_error, sizeof(detail::cuda_device_error));
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
        const cudaError_t status = cudaDeviceSynchronize();
        detail::cuda_device_error err;
        cudaMemcpyFromSymbol(&err, detail::g_device_error, sizeof(detail::cuda_device_error));
        reset();

        if (err.flag != 0)
        {
            std::ostringstream oss;
            oss << "Device error on thread " << err.thread_id
                << " at " << err.file
                << ":" << err.line
                << ": " << err.expression;

            // Need to clear our trap
            cudaGetLastError();

            // TODO(mborland): Can we get the type of exception to throw? e.g., overflow, underflow, range.
            BOOST_THROW_EXCEPTION(std::runtime_error(oss.str()));
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

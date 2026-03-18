// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP
#define BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP

#include <boost/safe_numbers/detail/config.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <cstdio>
#include <cmath>
#include <sstream>

#ifdef __NVCC__
#include <cuda_runtime.h>
#endif

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

struct cuda_device_error
{
    int         flag;       // 0 = no error, 1 = error captured
    int         line;       // __LINE__
    int         thread_id;  // Approximated by blockIdx.x * blockDim.x + threadIdx.x;
    const char* file;       // __FILE__ string literal
    const char* expression; // #x stringified expression
};

#ifdef __CUDA_ARCH__

__device__ cuda_device_error g_device_error = {0, 0, 0, nullptr, nullptr};

__device__ inline void report_device_error(
    const char* file,
    int line,
    const char* expression)
{
    if (atomicCAS(&g_device_error.flag, 0, 1) == 0)
    {
        g_device_error.file       = file;
        g_device_error.line       = line;
        g_device_error.expression = expression;
        g_device_error.thread_id  = blockIdx.x * blockDim.x + threadIdx.x;
        __threadfence_system();
    }

    // Print in case things something goes bad in PTX return from __trap
    printf("Device error at: [GPU thread %d] %s:%d: %s\n",
           blockIdx.x * blockDim.x + threadIdx.x,
           file, line, expression);

    __trap();
}

#endif // __CUDA_ARCH__

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP

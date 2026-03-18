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

#ifdef __NVCC__

__device__ inline void report_device_error(
    cuda_device_error* err,
    const char* file,
    int line,
    const char* expression)
{
    if (atomicCAS(&err->flag, 0, 1) == 0)
    {
        err->file       = file;
        err->line       = line;
        err->expression = expression;
        err->thread_id  = blockIdx.x * blockDim.x + threadIdx.x;
        __threadfence_system();
    }

    // Print in case things something goes bad in PTX return from __trap
    printf("Device error at: [GPU thread %d] %s:%d: %s\n",
           blockIdx.x * blockDim.x + threadIdx.x,
           file, line, expression);

    __trap();
}

#endif // __NVCC__

} // namespace boost::safe_numbers::detail

#endif // BOOST_SAFE_NUMBERS_CUDA_ERROR_REPORTING_HPP

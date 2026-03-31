// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <limits>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/detail/cuda_error_reporting.hpp>

#include <cuda_runtime.h>

using test_type = boost::safe_numbers::u32;
using basis_type = test_type::basis_type;

// This kernel deliberately overflows: it adds 1 to the maximum u32 value
__global__ void overflow_kernel(test_type* out)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i == 0)
    {
        const test_type max_val {(std::numeric_limits<basis_type>::max)()};
        out[0] = max_val + test_type{1};  // Overflow!
    }
}

int main()
{
    boost::safe_numbers::device_error_context ctx;

    test_type* result = nullptr;
    cudaMallocManaged(&result, sizeof(test_type));
    cudaDeviceSynchronize();

    std::cout << "=== Launching kernel that overflows ===" << std::endl;

    overflow_kernel<<<1, 1>>>(result);

    try
    {
        ctx.synchronize();
        std::cout << "No error detected (unexpected)" << std::endl;
    }
    catch (const std::overflow_error& e)
    {
        std::cout << "Caught overflow_error: " << e.what() << std::endl;
    }

    // Since we have trapped, the context is now unrecoverable until the process terminates
    const auto status = cudaDeviceSynchronize();
    if (status != cudaSuccess)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

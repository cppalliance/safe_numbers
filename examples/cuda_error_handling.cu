// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example demonstrates how to catch arithmetic errors that occur
// on a CUDA device using device_error_context. When a safe_numbers
// operation overflows on the GPU, the error is captured in managed
// memory and rethrown with BOOST_THROW_EXCEPTION on the host when
// you call ctx.synchronize().
//
// The device_error_context manages a dynamically allocated managed
// memory buffer. When an error is detected, synchronize() clears the
// error state and throws. After catching the exception, the same
// context can be reused immediately for new kernel launches.

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

// This kernel performs valid arithmetic
__global__ void safe_kernel(const test_type* in, test_type* out, int n)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < n)
    {
        out[i] = in[i] + test_type{1};
    }
}

int main()
{
    // Create a single device_error_context for the lifetime of the program.
    // The constructor allocates managed memory for error reporting and
    // clears any stale state.
    boost::safe_numbers::device_error_context ctx(boost::safe_numbers::throw_exception);

    // ---------------------------------------------------------------
    // Step 1: Launch a kernel that overflows and catch the error
    // ---------------------------------------------------------------

    test_type* result = nullptr;
    cudaMallocManaged(&result, sizeof(test_type));
    cudaDeviceSynchronize();

    std::cout << "=== Launching kernel that overflows ===" << std::endl;

    overflow_kernel<<<1, 1>>>(result);

    // synchronize() waits for the kernel, reads the error state,
    // and throws the appropriate std::exception if one was captured.
    // On error it clears the error state before throwing, so the
    // context is immediately reusable after catching the exception.
    try
    {
        ctx.synchronize();
        std::cout << "No error detected (unexpected)" << std::endl;
    }
    catch (const std::overflow_error& e)
    {
        std::cout << "Caught overflow_error: " << e.what() << std::endl;
    }

    // ---------------------------------------------------------------
    // Step 2: After catching the error, the same ctx can be reused
    //         immediately. synchronize() already cleared the error
    //         state before throwing, so no recovery step is needed.
    // ---------------------------------------------------------------

    std::cout << "\n=== Launching kernel with valid arithmetic ===" << std::endl;

    test_type* data = nullptr;
    test_type* out = nullptr;

    cudaMallocManaged(&data, 4 * sizeof(test_type));
    cudaMallocManaged(&out, 4 * sizeof(test_type));
    cudaDeviceSynchronize();

    data[0] = test_type{10};
    data[1] = test_type{20};
    data[2] = test_type{30};
    data[3] = test_type{40};

    safe_kernel<<<1, 4>>>(data, out, 4);

    try
    {
        ctx.synchronize();
        std::cout << "No error detected (expected)" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "Unexpected error: " << e.what() << std::endl;
    }

    for (int i = 0; i < 4; ++i)
    {
        std::cout << "result[" << i << "] = "
                  << static_cast<basis_type>(out[i]) << std::endl;
    }

    // ---------------------------------------------------------------
    // Cleanup
    // ---------------------------------------------------------------

    cudaFree(result);
    cudaFree(data);
    cudaFree(out);

    return 0;
}

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
#include <boost/safe_numbers/cuda_error_reporting.hpp>

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
    // ---------------------------------------------------------------
    // Step 1: Launch a kernel that overflows
    //
    // Instead of using the booost::safe_numbers::cuda_error_context
    // we will use standard CUDA machinery in case the user forgets
    // or does not want to use it.
    // ---------------------------------------------------------------

    test_type* result = nullptr;
    cudaMallocManaged(&result, sizeof(test_type));
    cudaDeviceSynchronize();

    std::cout << "=== Launching kernel that overflows ===" << std::endl;

    overflow_kernel<<<1, 1>>>(result);
    auto status = cudaDeviceSynchronize();

    // The default should have trapped the kernel, leading to failure conditions
    // Attempt to reset the device (which we know fails)
    if (status != cudaSuccess)
    {
        const auto error = cudaGetLastError();
        std::cerr << "Kernel failed with error: " << cudaGetErrorString(error) << std::endl;
        cudaDeviceReset();
        std::cerr << "Kernel has been reset via CUDA API" << std::endl;
    }

    // ---------------------------------------------------------------
    // Step 2: Try to launch another kernel from the same process
    //
    // Since we have trapped the CUDA device in the background,
    // it is unable to be launched again from the same process.
    // This error cannot be cleared even though we called cudaDeviceReset()
    // ---------------------------------------------------------------

    std::cout << "\n=== Launching kernel with valid arithmetic ===" << std::endl;

    test_type* data = nullptr;
    test_type* out = nullptr;

    cudaMallocManaged(&data, 4 * sizeof(test_type));
    cudaMallocManaged(&out, 4 * sizeof(test_type));
    status = cudaDeviceSynchronize();

    if (status != cudaSuccess)
    {
        const auto error = cudaGetLastError();
        std::cerr << "Kernel failed with error: " << cudaGetErrorString(error) << std::endl;
    }
    else
    {
        // If we had not checked status, this following code would have terminated with SegFault at data[0]

        data[0] = test_type{10};
        data[1] = test_type{20};
        data[2] = test_type{30};
        data[3] = test_type{40};

        safe_kernel<<<1, 4>>>(data, out, 4);
        status = cudaDeviceSynchronize();

        if (status != cudaSuccess)
        {
            const auto error = cudaGetLastError();
            std::cerr << "Kernel failed with error: " << cudaGetErrorString(error) << std::endl;
        }
    }

    // ---------------------------------------------------------------
    // Cleanup
    // ---------------------------------------------------------------

    if (result != nullptr)
    {
        cudaFree(result);
    }
    if (data != nullptr)
    {
        cudaFree(data);
    }
    if (out != nullptr)
    {
        cudaFree(out);
    }

    return 0;
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/bit.hpp>
#include <boost/safe_numbers/integer_utilities.hpp>
#include <boost/safe_numbers/numeric.hpp>
#include <boost/safe_numbers/charconv.hpp>
#include <boost/safe_numbers/cuda_error_reporting.hpp>

#include <cuda_runtime.h>

using test_type = boost::safe_numbers::u32;
using basis_type = test_type::basis_type;

// All safe_numbers types and free functions are annotated with __host__ __device__,
// so they work identically on both host and device.

__global__ void arithmetic_kernel(const test_type* a, const test_type* b, test_type* out, int n)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < n)
    {
        // Basic arithmetic with overflow detection works on device
        out[i] = a[i] + b[i];
    }
}

__global__ void bit_kernel(const test_type* in, int* out, int n)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < n)
    {
        // All <bit> free functions work on device
        out[i] = boost::safe_numbers::popcount(in[i]);
    }
}

__global__ void utility_kernel(const test_type* in, test_type* out, int n)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < n)
    {
        // Integer utilities work on device
        out[i] = boost::safe_numbers::isqrt(in[i]);
    }
}

__global__ void numeric_kernel(const test_type* a, const test_type* b, test_type* out, int n)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < n)
    {
        // gcd, lcm, midpoint work on device
        out[i] = boost::safe_numbers::gcd(a[i], b[i]);
    }
}

__global__ void charconv_kernel(const test_type* in, test_type* out, int n)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < n)
    {
        // charconv round-trip on device
        char buf[16] {};
        auto tc = boost::charconv::to_chars(buf, buf + sizeof(buf), in[i]);
        test_type parsed {};
        boost::charconv::from_chars(buf, tc.ptr, parsed);
        out[i] = parsed;
    }
}

// Helper: allocate CUDA managed memory
void allocate(void** ptr, std::size_t bytes)
{
    cudaError_t err = cudaMallocManaged(ptr, bytes);
    if (err != cudaSuccess)
    {
        throw std::runtime_error(cudaGetErrorString(err));
    }
    cudaDeviceSynchronize();
}

template <typename T>
void cleanup(T** ptr)
{
    if (*ptr != nullptr)
    {
        cudaFree(*ptr);
        *ptr = nullptr;
    }
}

int main()
{
    const int n = 10000;
    const int threadsPerBlock = 256;
    const int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;

    std::mt19937_64 rng {42};
    std::uniform_int_distribution<basis_type> dist {basis_type{1}, (std::numeric_limits<basis_type>::max)() / basis_type{2}};

    // --- Allocate managed arrays ---

    test_type* a = nullptr;
    test_type* b = nullptr;
    test_type* out_tt = nullptr;
    int* out_int = nullptr;

    allocate(reinterpret_cast<void**>(&a), n * sizeof(test_type));
    allocate(reinterpret_cast<void**>(&b), n * sizeof(test_type));
    allocate(reinterpret_cast<void**>(&out_tt), n * sizeof(test_type));
    allocate(reinterpret_cast<void**>(&out_int), n * sizeof(int));

    for (int i = 0; i < n; ++i)
    {
        a[i] = test_type{dist(rng)};
        b[i] = test_type{dist(rng)};
    }

    // The device_error_context captures any overflow/underflow errors
    // reported from device code and rethrows them on the host.
    boost::safe_numbers::device_error_context ctx;

    // --- Test 1: Arithmetic (a + b, using half-range to avoid overflow) ---

    arithmetic_kernel<<<blocksPerGrid, threadsPerBlock>>>(a, b, out_tt, n);
    ctx.synchronize();

    bool pass = true;
    for (int i = 0; i < n; ++i)
    {
        if (out_tt[i] != a[i] + b[i])
        {
            pass = false;
            break;
        }
    }
    std::cout << "Arithmetic (add):  " << (pass ? "PASSED" : "FAILED") << '\n';

    // --- Test 2: Bit functions (popcount) ---

    bit_kernel<<<blocksPerGrid, threadsPerBlock>>>(a, out_int, n);
    ctx.synchronize();

    pass = true;
    for (int i = 0; i < n; ++i)
    {
        if (out_int[i] != boost::safe_numbers::popcount(a[i]))
        {
            pass = false;
            break;
        }
    }
    std::cout << "Bit (popcount):    " << (pass ? "PASSED" : "FAILED") << '\n';

    // --- Test 3: Integer utilities (isqrt) ---

    utility_kernel<<<blocksPerGrid, threadsPerBlock>>>(a, out_tt, n);
    ctx.synchronize();

    pass = true;
    for (int i = 0; i < n; ++i)
    {
        if (out_tt[i] != boost::safe_numbers::isqrt(a[i]))
        {
            pass = false;
            break;
        }
    }
    std::cout << "Utility (isqrt):   " << (pass ? "PASSED" : "FAILED") << '\n';

    // --- Test 4: Numeric (gcd) ---

    numeric_kernel<<<blocksPerGrid, threadsPerBlock>>>(a, b, out_tt, n);
    ctx.synchronize();

    pass = true;
    for (int i = 0; i < n; ++i)
    {
        if (out_tt[i] != boost::safe_numbers::gcd(a[i], b[i]))
        {
            pass = false;
            break;
        }
    }
    std::cout << "Numeric (gcd):     " << (pass ? "PASSED" : "FAILED") << '\n';

    // --- Test 5: Charconv round-trip ---

    charconv_kernel<<<blocksPerGrid, threadsPerBlock>>>(a, out_tt, n);
    ctx.synchronize();

    pass = true;
    for (int i = 0; i < n; ++i)
    {
        if (out_tt[i] != a[i])
        {
            pass = false;
            break;
        }
    }
    std::cout << "Charconv (rt):     " << (pass ? "PASSED" : "FAILED") << '\n';

    // --- Cleanup ---

    cleanup(&a);
    cleanup(&b);
    cleanup(&out_tt);
    cleanup(&out_int);
    cudaDeviceReset();

    return 0;
}

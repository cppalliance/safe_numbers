//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <random>
#include <cstdint>
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/cuda_error_reporting.hpp>
#include "cuda_managed_ptr.hpp"

#include <cuda_runtime.h>

using test_type = boost::safe_numbers::bounded_int<-1000000, 1000000>;

__global__ void cuda_test(const test_type *a, const test_type *b, test_type *out, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        out[i] = a[i] + b[i];
    }
}

int main()
{
    int numElements = 2048;

    cuda_managed_ptr<test_type> A(numElements);
    cuda_managed_ptr<test_type> B(numElements);
    cuda_managed_ptr<test_type> OUT(numElements);

    std::mt19937_64 rng{42};
    std::uniform_int_distribution<std::int32_t> dist{-400000, 400000};
    for (int i = 0; i < numElements; ++i)
    {
        A[i] = test_type{dist(rng)};
        B[i] = test_type{dist(rng)};
    }

    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;

    boost::safe_numbers::device_error_context ctx;
    cuda_test<<<blocksPerGrid, threadsPerBlock>>>(A.get(), B.get(), OUT.get(), numElements);
    ctx.synchronize();

    for (int i = 0; i < numElements; ++i)
    {
        if (OUT[i] != A[i] + B[i])
        {
            std::cerr << "Result verification failed at element " << i << "!" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "Test PASSED\n";
    return 0;
}

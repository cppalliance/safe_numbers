//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <stdexcept>
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/cuda_error_reporting.hpp>
#include "cuda_managed_ptr.hpp"

#include <cuda_runtime.h>

using test_type = boost::safe_numbers::bounded_int<-1000, 1000>;

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
    int numElements = 1024;

    cuda_managed_ptr<test_type> A(numElements);
    cuda_managed_ptr<test_type> B(numElements);
    cuda_managed_ptr<test_type> OUT(numElements);

    // 600 + 600 = 1200 exceeds the [-1000, 1000] bound -> out of range
    for (int i = 0; i < numElements; ++i)
    {
        A[i] = test_type{600};
        B[i] = test_type{600};
    }

    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;

    boost::safe_numbers::device_error_context ctx;
    cuda_test<<<blocksPerGrid, threadsPerBlock>>>(A.get(), B.get(), OUT.get(), numElements);

    try
    {
        ctx.synchronize();
        std::cerr << "Expected exception from out-of-range result but none was thrown!" << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::domain_error& e)
    {
        std::cerr << "Caught expected error: " << e.what() << std::endl;
        std::cerr << "Test PASSED\n";
        return 0;
    }
}

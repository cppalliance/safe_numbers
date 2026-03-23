//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)



#include <iostream>
#include <stdexcept>
#include <limits>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/detail/cuda_error_reporting.hpp>
#include "cuda_managed_ptr.hpp"

#include <cuda_runtime.h>

using test_type = boost::safe_numbers::u16;
using basis_type = test_type::basis_type;

__global__ void cuda_test(const test_type *in, test_type *out, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        out[i] = in[i] + in[i];
    }
}

int main(void)
{
    int numElements = 1024;

    cuda_managed_ptr<test_type> input_vector(numElements);
    cuda_managed_ptr<test_type> output_vector(numElements);

    for (int i = 0; i < numElements; ++i)
    {
        input_vector[i] = test_type{(std::numeric_limits<basis_type>::max)()};
    }

    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;

    boost::safe_numbers::device_error_context ctx;

    cuda_test<<<blocksPerGrid, threadsPerBlock>>>(input_vector.get(), output_vector.get(), numElements);

    try
    {
        ctx.synchronize();
        std::cerr << "Expected exception from overflow but none was thrown!" << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "Caught expected error: " << e.what() << std::endl;
        std::cout << "Test PASSED\n";
        return 0;
    }
}

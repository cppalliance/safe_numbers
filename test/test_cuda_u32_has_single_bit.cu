//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <limits>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/bit.hpp>
#include <boost/safe_numbers/detail/cuda_error_reporting.hpp>
#include "cuda_managed_ptr.hpp"
#include "stopwatch.hpp"

#include <cuda_runtime.h>

using test_type = boost::safe_numbers::u32;
using basis_type = test_type::basis_type;

__global__ void cuda_test(const test_type *in, int *out, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        out[i] = static_cast<int>(boost::safe_numbers::has_single_bit(in[i]));
    }
}

int main(void)
{
    std::mt19937_64 rng{42};

    int numElements = 50000;
    std::cout << "[Vector operation on " << numElements << " elements]" << std::endl;

    cuda_managed_ptr<test_type> input_vector(numElements);
    cuda_managed_ptr<int> output_vector(numElements);

    std::uniform_int_distribution<basis_type> dist{basis_type{0}, (std::numeric_limits<basis_type>::max)()};
    for (int i = 0; i < numElements; ++i)
    {
        input_vector[i] = test_type{dist(rng)};
    }

    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;
    std::cout << "CUDA kernel launch with " << blocksPerGrid << " blocks of " << threadsPerBlock << " threads" << std::endl;

    boost::safe_numbers::device_error_context ctx;
    watch w;

    cuda_test<<<blocksPerGrid, threadsPerBlock>>>(input_vector.get(), output_vector.get(), numElements);
    ctx.synchronize();

    std::cout << "CUDA kernel done in: " << w.elapsed() << "s" << std::endl;

    std::vector<int> results;
    results.reserve(numElements);
    w.reset();
    for (int i = 0; i < numElements; ++i)
    {
        results.push_back(static_cast<int>(boost::safe_numbers::has_single_bit(input_vector[i])));
    }
    double t = w.elapsed();

    for (int i = 0; i < numElements; ++i)
    {
        if (output_vector[i] != results[i])
        {
            std::cerr << "Result verification failed at element " << i << "!" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "Test PASSED, normal calculation time: " << t << "s" << std::endl;
    std::cout << "Done\n";

    return 0;
}

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
#include <boost/safe_numbers/integer_utilities.hpp>
#include <boost/safe_numbers/cuda_error_reporting.hpp>
#include "cuda_managed_ptr.hpp"
#include "stopwatch.hpp"

#include <cuda_runtime.h>

using test_type = boost::safe_numbers::u8;
using basis_type = test_type::basis_type;

__global__ void cuda_test(const test_type *in1, const test_type *in2, test_type *out, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        out[i] = boost::safe_numbers::abs_diff(in1[i], in2[i]);
    }
}

int main(void)
{
    std::mt19937_64 rng{42};

    int numElements = 50000;
    std::cout << "[Vector operation on " << numElements << " elements]" << std::endl;

    cuda_managed_ptr<test_type> input_vector1(numElements);
    cuda_managed_ptr<test_type> input_vector2(numElements);
    cuda_managed_ptr<test_type> output_vector(numElements);

    std::uniform_int_distribution<unsigned> dist{0U, static_cast<unsigned>((std::numeric_limits<basis_type>::max)())};
    std::uniform_int_distribution<unsigned> dist2{0U, static_cast<unsigned>((std::numeric_limits<basis_type>::max)())};
    for (int i = 0; i < numElements; ++i)
    {
        input_vector1[i] = test_type{static_cast<basis_type>(dist(rng))};
        input_vector2[i] = test_type{static_cast<basis_type>(dist2(rng))};
    }

    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;
    std::cout << "CUDA kernel launch with " << blocksPerGrid << " blocks of " << threadsPerBlock << " threads" << std::endl;

    boost::safe_numbers::device_error_context ctx;
    watch w;

    cuda_test<<<blocksPerGrid, threadsPerBlock>>>(input_vector1.get(), input_vector2.get(), output_vector.get(), numElements);
    ctx.synchronize();

    std::cout << "CUDA kernel done in: " << w.elapsed() << "s" << std::endl;

    std::vector<test_type> results;
    results.reserve(numElements);
    w.reset();
    for (int i = 0; i < numElements; ++i)
    {
        results.push_back(boost::safe_numbers::abs_diff(input_vector1[i], input_vector2[i]));
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

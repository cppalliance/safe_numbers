//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_INT128_ALLOW_SIGN_CONVERSION
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION

#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <limits>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/cuda_error_reporting.hpp>
#include <boost/safe_numbers/detail/int128/random.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "cuda_managed_ptr.hpp"
#include "stopwatch.hpp"

#include <cuda_runtime.h>

using test_type = boost::safe_numbers::i128;
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
    std::mt19937_64 rng{42};

    int numElements = 50000;
    std::cout << "[Vector operation on " << numElements << " elements]" << std::endl;

    cuda_managed_ptr<test_type> input_vector(numElements);
    cuda_managed_ptr<test_type> output_vector(numElements);

    boost::random::uniform_int_distribution<basis_type> dist{(std::numeric_limits<basis_type>::min)() / 2, (std::numeric_limits<basis_type>::max)() / 2};
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

    std::vector<test_type> results;
    results.reserve(numElements);
    w.reset();
    for (int i = 0; i < numElements; ++i)
    {
        results.push_back(input_vector[i] + input_vector[i]);
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

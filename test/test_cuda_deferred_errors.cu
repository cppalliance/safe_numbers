//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//  Exercises deferred_errors: overflowing_* ops accumulate their flags
//  branch-free and report() surfaces the result once per thread through the
//  same machinery as the throwing operators.

#include <iostream>
#include <stdexcept>
#include <limits>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/device_error_reporting.hpp>
#include "cuda_managed_ptr.hpp"

#include <cuda_runtime.h>

using test_type = boost::safe_numbers::u32;
using basis_type = test_type::basis_type;

__global__ void deferred_kernel(const test_type *in, test_type *out, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        boost::safe_numbers::deferred_errors errs;

        auto x {errs.unwrap(boost::safe_numbers::overflowing_add(in[i], in[i]))};
        x = errs.unwrap(boost::safe_numbers::overflowing_mul(x, test_type{basis_type{2}}));
        x = errs.unwrap(boost::safe_numbers::overflowing_sub(x, in[i]));

        errs.report(__FILE__, __LINE__);
        out[i] = x;
    }
}

int main(void)
{
    int numElements = 1024;
    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;

    cuda_managed_ptr<test_type> input_vector(numElements);
    cuda_managed_ptr<test_type> output_vector(numElements);

    boost::safe_numbers::device_error_context ctx;

    // Happy path: values stay in range, no flag is ever set
    for (int i = 0; i < numElements; ++i)
    {
        input_vector[i] = test_type{static_cast<basis_type>(i % 1000 + 1)};
    }

    deferred_kernel<<<blocksPerGrid, threadsPerBlock>>>(input_vector.get(), output_vector.get(), numElements);

    try
    {
        ctx.synchronize();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unexpected error on the happy path: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    for (int i = 0; i < numElements; ++i)
    {
        const auto v {static_cast<basis_type>(i % 1000 + 1)};
        if (static_cast<basis_type>(output_vector[i]) != static_cast<basis_type>(v * 4 - v))
        {
            std::cerr << "Result verification failed at element " << i << "!" << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Error path: max + max sets the deferred flag and report() records it
    for (int i = 0; i < numElements; ++i)
    {
        input_vector[i] = test_type{(std::numeric_limits<basis_type>::max)()};
    }

    deferred_kernel<<<blocksPerGrid, threadsPerBlock>>>(input_vector.get(), output_vector.get(), numElements);

    try
    {
        ctx.synchronize();
        std::cerr << "Expected exception from deferred report but none was thrown!" << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::overflow_error& e)
    {
        std::cerr << "Caught expected error: " << e.what() << std::endl;
        std::cerr << "Test PASSED\n";
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Wrong exception type: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

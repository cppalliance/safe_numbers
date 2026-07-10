//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Exercises the remaining bounded-type operators on the device (modulo,
// increment/decrement, unary plus/minus, and compound assignment) to
// confirm they are all callable from device code and produce correct results.

#include <iostream>
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/bounded_floats.hpp>
#include <boost/safe_numbers/device_error_reporting.hpp>
#include "cuda_managed_ptr.hpp"

#include <cuda_runtime.h>

using bu = boost::safe_numbers::bounded_uint<0u, 1000u>;
using bi = boost::safe_numbers::bounded_int<-1000, 1000>;
using bf = boost::safe_numbers::bounded_float<-1000.0f, 1000.0f>;

__global__ void ops_kernel(int* ok)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i == 0)
    {
        // bounded_uint: modulo, increment, decrement, compound assignment
        const bu a {100};
        const bu b {30};
        bu inc {a};
        ++inc;
        inc--;
        bu comp {a};
        comp += b;
        comp -= b;
        comp *= bu{2};
        comp /= bu{2};
        const bool r1 {(a % b == bu{10}) && (inc == a) && (comp == a) && (a > b)};

        // bounded_int: modulo, unary plus/minus, increment/decrement
        const bi x {-100};
        const bi y {7};
        const bi neg {-y};
        bi xc {x};
        ++xc;
        --xc;
        const bool r2 {(x % y == bi{-2}) && (neg == bi{-7}) && (+x == x) && (xc == x)};

        // bounded_float: compound assignment and comparison
        const bf p {bf::basis_type{2.0f}};
        const bf q {bf::basis_type{3.0f}};
        bf pc {p};
        pc *= q;
        pc /= q;
        const bool r3 {(pc == p) && (p < q)};

        *ok = (r1 && r2 && r3) ? 1 : 0;
    }
}

int main()
{
    int* ok {nullptr};
    cudaMallocManaged(&ok, sizeof(int));
    cudaDeviceSynchronize();
    *ok = 0;

    boost::safe_numbers::device_error_context ctx;
    ops_kernel<<<1, 1>>>(ok);
    ctx.synchronize();

    if (*ok == 1)
    {
        std::cout << "Test PASSED\n";
        cudaFree(ok);
        return 0;
    }

    std::cerr << "Result verification failed!" << std::endl;
    cudaFree(ok);
    return EXIT_FAILURE;
}

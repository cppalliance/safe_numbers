// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example demonstrates the range-checked bounded types
// (bounded_uint, bounded_int, bounded_float) running on a CUDA device.
// In-range arithmetic is verified against the host result, and an
// out-of-range result is caught on the host through device_error_context.

#include <iostream>
#include <stdexcept>
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/bounded_floats.hpp>
#include <boost/safe_numbers/cuda_error_reporting.hpp>

#include <cuda_runtime.h>

using bu = boost::safe_numbers::bounded_uint<0u, 1000000u>;
using bi = boost::safe_numbers::bounded_int<-1000000, 1000000>;
using bf = boost::safe_numbers::bounded_float<0.0f, 1000000.0f>;

__global__ void uint_add(const bu* a, const bu* b, bu* out)   { *out = *a + *b; }
__global__ void int_mul(const bi* a, const bi* b, bi* out)    { *out = *a * *b; }
__global__ void float_add(const bf* a, const bf* b, bf* out)  { *out = *a + *b; }

template <typename T>
T* managed()
{
    T* p = nullptr;
    cudaMallocManaged(&p, sizeof(T));
    cudaDeviceSynchronize();
    return p;
}

int main()
{
    // untrapped mode: a device error is deferred to the host rather than
    // trapping the kernel, so the context can be reused after catching.
    boost::safe_numbers::device_error_context ctx {boost::safe_numbers::untrapped};

    // bounded_uint: 300000 + 400000 = 700000, within [0, 1000000]
    bu* ua {managed<bu>()};
    bu* ub {managed<bu>()};
    bu* uo {managed<bu>()};
    *ua = bu{300000};
    *ub = bu{400000};
    uint_add<<<1, 1>>>(ua, ub, uo);
    ctx.synchronize();
    std::cout << "bounded_uint  add: " << ((*uo == *ua + *ub) ? "PASSED" : "FAILED") << std::endl;

    // bounded_int: -600 * 700 = -420000, within [-1000000, 1000000]
    bi* ia {managed<bi>()};
    bi* ib {managed<bi>()};
    bi* io {managed<bi>()};
    *ia = bi{-600};
    *ib = bi{700};
    int_mul<<<1, 1>>>(ia, ib, io);
    ctx.synchronize();
    std::cout << "bounded_int   mul: " << ((*io == *ia * *ib) ? "PASSED" : "FAILED") << std::endl;

    // bounded_float: 250.5 + 749.25 = 999.75, within [0, 1000000]
    bf* fa {managed<bf>()};
    bf* fb {managed<bf>()};
    bf* fo {managed<bf>()};
    *fa = bf{bf::basis_type{250.5f}};
    *fb = bf{bf::basis_type{749.25f}};
    float_add<<<1, 1>>>(fa, fb, fo);
    ctx.synchronize();
    std::cout << "bounded_float add: " << ((*fo == *fa + *fb) ? "PASSED" : "FAILED") << std::endl;

    // Out of range: 800000 + 800000 = 1600000 exceeds the [0, 1000000] bound
    *ua = bu{800000};
    *ub = bu{800000};
    uint_add<<<1, 1>>>(ua, ub, uo);
    try
    {
        ctx.synchronize();
        std::cout << "out-of-range: no error thrown (unexpected)" << std::endl;
    }
    catch (const std::domain_error&)
    {
        std::cout << "out-of-range sum:  caught std::domain_error" << std::endl;
    }

    cudaFree(ua);
    cudaFree(ub);
    cudaFree(uo);
    cudaFree(ia);
    cudaFree(ib);
    cudaFree(io);
    cudaFree(fa);
    cudaFree(fb);
    cudaFree(fo);

    return 0;
}

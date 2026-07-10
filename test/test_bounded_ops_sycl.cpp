//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  Exercises the remaining bounded-type operators on the device (modulo,
//  increment/decrement, unary plus/minus, and compound assignment) to
//  confirm they are all callable from device code and produce correct
//  results (the SYCL analog of the CUDA bounded_ops test).

#include "sycl_test.hpp"
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/bounded_floats.hpp>

using bu = boost::safe_numbers::bounded_uint<0U, 1000U>;
using bi = boost::safe_numbers::bounded_int<-1000, 1000>;
using bf = boost::safe_numbers::bounded_float<-1000.0F, 1000.0F>;

int main()
{
    sycl::queue q;

    auto* ok {sycl::malloc_shared<int>(1, q)};
    *ok = 0;

    q.single_task([=]()
    {
        // bounded_uint: modulo, increment, decrement, compound assignment
        const bu a {100U};
        const bu b {30U};
        bu inc {a};
        ++inc;
        inc--;
        bu comp {a};
        comp += b;
        comp -= b;
        comp *= bu{2U};
        comp /= bu{2U};
        const bool r1 {(a % b == bu{10U}) && (inc == a) && (comp == a) && (a > b)};

        // bounded_int: modulo, unary plus/minus, increment/decrement
        const bi x {-100};
        const bi y {7};
        const bi neg {-y};
        bi xc {x};
        ++xc;
        --xc;
        const bool r2 {(x % y == bi{-2}) && (neg == bi{-7}) && (+x == x) && (xc == x)};

        // bounded_float: compound assignment and comparison
        const bf p {bf::basis_type{2.0F}};
        const bf v {bf::basis_type{3.0F}};
        bf pc {p};
        pc *= v;
        pc /= v;
        const bool r3 {(pc == p) && (p < v)};

        *ok = (r1 && r2 && r3) ? 1 : 0;
    }).wait();

    const bool passed {*ok == 1};
    sycl::free(ok, q);

    std::cout << "bounded ops: " << (passed ? "PASSED" : "FAILED") << "\n";
    return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}

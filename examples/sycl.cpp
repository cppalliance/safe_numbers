// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Demonstrates running Boost.safe_numbers arithmetic on a SYCL device.
// Build (Intel oneAPI): icpx -fsycl -DBOOST_SAFE_NUMBERS_ENABLE_SYCL=1 sycl.cpp
//
// <sycl/sycl.hpp> must be included before any safe_numbers header so that the
// SYCL_EXTERNAL keyword is available.

#include <sycl/sycl.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>

#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

using boost::safe_numbers::u128;

int main()
{
    sycl::queue q;
    std::cout << "Running on: "
              << q.get_device().get_info<sycl::info::device::name>() << "\n";

    constexpr int num_elements {50000};

    u128* in1 {sycl::malloc_shared<u128>(num_elements, q)};
    u128* in2 {sycl::malloc_shared<u128>(num_elements, q)};
    u128* out {sycl::malloc_shared<u128>(num_elements, q)};

    // Inputs kept well below the 128-bit maximum so 2*a + b cannot overflow.
    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        in1[i] = u128{rng()};
        in2[i] = u128{rng()};
    }

    q.submit([&](sycl::handler& h)
    {
        h.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
        {
            const int i {static_cast<int>(idx[0])};
            out[i] = in1[i] * u128{2} + in2[i];
        });
    }).wait();

    std::vector<u128> expected;
    expected.reserve(num_elements);
    for (int i {0}; i < num_elements; ++i)
    {
        expected.push_back(in1[i] * u128{2} + in2[i]);
    }

    bool all_match {true};
    for (int i {0}; i < num_elements; ++i)
    {
        if (out[i] != expected[i])
        {
            all_match = false;
            break;
        }
    }

    sycl::free(in1, q);
    sycl::free(in2, q);
    sycl::free(out, q);

    if (all_match)
    {
        std::cout << "All CPU and GPU computed elements match!\n";
        return EXIT_SUCCESS;
    }

    std::cout << "Mismatch between CPU and GPU results\n";
    return EXIT_FAILURE;
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Demonstrates Boost.safe_numbers device error handling under SYCL.
// A safe_numbers operation cannot throw on a device, so an error (here a
// bounded value leaving its range) is recorded on the device and re-raised on
// the host as a normal std::exception by device_error_context::synchronize().
// Build (Intel oneAPI): icpx -fsycl -DBOOST_SAFE_NUMBERS_ENABLE_SYCL=1 sycl_error_handling.cpp

#include <sycl/sycl.hpp>
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/device_error_reporting.hpp>

#include <iostream>
#include <stdexcept>

using bu = boost::safe_numbers::bounded_uint<0u, 1000u>;

int main()
{
    sycl::queue q;
    std::cout << "Running on: "
              << q.get_device().get_info<sycl::info::device::name>() << "\n";

    constexpr int num_elements {1024};
    bu* a   {sycl::malloc_shared<bu>(num_elements, q)};
    bu* b   {sycl::malloc_shared<bu>(num_elements, q)};
    bu* out {sycl::malloc_shared<bu>(num_elements, q)};

    // 600 + 600 = 1200, which is outside the declared range [0, 1000].
    for (int i {0}; i < num_elements; ++i)
    {
        a[i] = bu{600};
        b[i] = bu{600};
    }

    // The context must be constructed around the launch and only one may exist
    // at a time. It binds to the queue used for the kernels.
    boost::safe_numbers::device_error_context ctx {q};

    q.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
    {
        const int i {static_cast<int>(idx[0])};
        out[i] = a[i] + b[i];
    });

    int result {EXIT_FAILURE};
    try
    {
        // Waits for the kernels, then throws if the device recorded an error.
        ctx.synchronize();
        std::cout << "No error was reported (unexpected)\n";
    }
    catch (const std::domain_error& e)
    {
        std::cout << "Caught expected device error on the host: " << e.what() << "\n";
        result = EXIT_SUCCESS;
    }

    sycl::free(a, q);
    sycl::free(b, q);
    sycl::free(out, q);
    return result;
}

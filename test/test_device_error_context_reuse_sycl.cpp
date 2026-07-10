//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  After synchronize() throws a captured device error, the context clears the
//  device buffer and must be immediately reusable for further launches (the
//  SYCL analog of the CUDA untrapped-mode reuse test).

#include "sycl_test.hpp"
#include <limits>

int main()
{
    using boost::safe_numbers::u32;

    sycl::queue q;
    boost::safe_numbers::device_error_context ctx {q};

    constexpr int n {64};
    auto* big {sycl::malloc_shared<u32>(n, q)};
    auto* out {sycl::malloc_shared<u32>(n, q)};
    for (int i {0}; i < n; ++i)
    {
        big[i] = u32{(std::numeric_limits<std::uint32_t>::max)()};
    }

    // First launch overflows and must throw on synchronize
    q.parallel_for(sycl::range<1>(n), [=](sycl::id<1> idx)
    {
        const int i {static_cast<int>(idx[0])};
        out[i] = big[i] + big[i];
    });

    bool caught {false};
    try
    {
        ctx.synchronize();
    }
    catch (const std::overflow_error&)
    {
        caught = true;
    }

    if (!caught)
    {
        std::cerr << "FAILED: no overflow_error from the first launch\n";
        sycl::free(big, q);
        sycl::free(out, q);
        return EXIT_FAILURE;
    }

    // Second launch on the same context must run cleanly and synchronize
    // without throwing
    q.parallel_for(sycl::range<1>(n), [=](sycl::id<1> idx)
    {
        const int i {static_cast<int>(idx[0])};
        out[i] = u32{1U} + u32{2U};
    });

    try
    {
        ctx.synchronize();
    }
    catch (const std::exception& e)
    {
        std::cerr << "FAILED: context not reusable: " << e.what() << "\n";
        sycl::free(big, q);
        sycl::free(out, q);
        return EXIT_FAILURE;
    }

    int failures {0};
    for (int i {0}; i < n; ++i)
    {
        if (!(out[i] == u32{3U}))
        {
            ++failures;
        }
    }

    sycl::free(big, q);
    sycl::free(out, q);

    std::cout << "context reuse: " << (failures == 0 ? "PASSED" : "FAILED") << "\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

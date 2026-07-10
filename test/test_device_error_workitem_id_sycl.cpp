//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  When exactly one element triggers a device error, the recorded work-item
//  id must identify it, so the failing input can be narrowed down exactly as
//  with the CUDA backend. Checked for both a plain range launch and an
//  nd_range launch.

#include "sycl_test.hpp"
#include <limits>
#include <string>

using boost::safe_numbers::u32;

constexpr int n {4096};

template <typename Launch>
int expect_thread_in_message(const char* label, const int bad_index, Launch launch)
{
    sycl::queue q;
    boost::safe_numbers::device_error_context ctx {q};

    auto* a   {sycl::malloc_shared<u32>(n, q)};
    auto* out {sycl::malloc_shared<u32>(n, q)};
    for (int i {0}; i < n; ++i)
    {
        // Only a[bad_index] overflows when doubled
        a[i] = (i == bad_index) ? u32{(std::numeric_limits<std::uint32_t>::max)()} : u32{1U};
    }

    launch(q, a, out);

    int rc {EXIT_FAILURE};
    try
    {
        ctx.synchronize();
        std::cerr << label << ": FAILED - no exception thrown\n";
    }
    catch (const std::overflow_error& e)
    {
        const std::string what {e.what()};
        const std::string expected {"thread " + std::to_string(bad_index)};
        if (what.find(expected) != std::string::npos)
        {
            std::cout << label << ": PASSED (" << what << ")\n";
            rc = EXIT_SUCCESS;
        }
        else
        {
            std::cerr << label << ": FAILED - expected '" << expected << "' in: " << what << "\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << label << ": FAILED - wrong exception type: " << e.what() << "\n";
    }

    sycl::free(a, q);
    sycl::free(out, q);
    return rc;
}

int main()
{
    int rc {EXIT_SUCCESS};

    rc |= expect_thread_in_message("plain range work-item id", 1234,
        [](sycl::queue& q, const u32* a, u32* out)
        {
            q.parallel_for(sycl::range<1>(n), [=](sycl::id<1> idx)
            {
                const int i {static_cast<int>(idx[0])};
                out[i] = a[i] + a[i];
            });
        });

    rc |= expect_thread_in_message("nd_range work-item id", 777,
        [](sycl::queue& q, const u32* a, u32* out)
        {
            q.parallel_for(sycl::nd_range<1>(sycl::range<1>(n), sycl::range<1>(64)),
                           [=](sycl::nd_item<1> it)
            {
                const int i {static_cast<int>(it.get_global_linear_id())};
                out[i] = a[i] + a[i];
            });
        });

    return rc;
}

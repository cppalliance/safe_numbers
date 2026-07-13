//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  Exercises deferred_errors on a SYCL device: overflowing_* ops accumulate
//  their flags branch-free inside the kernel and report() surfaces the result
//  once per work item through the same device_global machinery that the
//  throwing operators use.

#include "sycl_test.hpp"

using test_type = boost::safe_numbers::u32;
using basis_type = test_type::basis_type;

namespace {

sycl::event launch_deferred(sycl::queue& q, const test_type* in, test_type* out, const int n)
{
    return q.parallel_for(sycl::range<1>(static_cast<std::size_t>(n)), [=](sycl::id<1> idx)
    {
        const auto i {idx[0]};

        boost::safe_numbers::deferred_errors errs;

        auto x {errs.unwrap(boost::safe_numbers::overflowing_add(in[i], in[i]))};
        x = errs.unwrap(boost::safe_numbers::overflowing_mul(x, test_type{basis_type{2}}));
        x = errs.unwrap(boost::safe_numbers::overflowing_sub(x, in[i]));

        errs.report(__FILE__, __LINE__);
        out[i] = x;
    });
}

int test_happy_path()
{
    constexpr int n {1024};

    sycl::queue q;
    boost::safe_numbers::device_error_context ctx {q};

    auto* in {sycl::malloc_shared<test_type>(n, q)};
    auto* out {sycl::malloc_shared<test_type>(n, q)};
    for (int i {0}; i < n; ++i)
    {
        in[i] = test_type{static_cast<basis_type>(i % 1000 + 1)};
    }

    launch_deferred(q, in, out, n);

    auto rc {EXIT_SUCCESS};
    try
    {
        ctx.synchronize();

        auto failures {0};
        for (int i {0}; i < n; ++i)
        {
            const auto v {static_cast<basis_type>(i % 1000 + 1)};
            if (static_cast<basis_type>(out[i]) != static_cast<basis_type>(v * 4 - v))
            {
                ++failures;
            }
        }
        std::cout << "deferred happy path: " << (failures == 0 ? "PASSED" : "FAILED") << "\n";
        rc = failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        std::cerr << "deferred happy path: FAILED - unexpected error: " << e.what() << "\n";
        rc = EXIT_FAILURE;
    }

    sycl::free(in, q);
    sycl::free(out, q);
    return rc;
}

int test_error_path()
{
    constexpr int n {1024};

    sycl::queue q;
    boost::safe_numbers::device_error_context ctx {q};

    auto* in {sycl::malloc_shared<test_type>(n, q)};
    auto* out {sycl::malloc_shared<test_type>(n, q)};
    for (int i {0}; i < n; ++i)
    {
        in[i] = test_type{(std::numeric_limits<basis_type>::max)()};
    }

    launch_deferred(q, in, out, n);

    auto rc {EXIT_FAILURE};
    try
    {
        ctx.synchronize();
        std::cerr << "deferred error path: FAILED - no exception thrown\n";
    }
    catch (const std::overflow_error& e)
    {
        std::cout << "deferred error path: PASSED (" << e.what() << ")\n";
        rc = EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << "deferred error path: FAILED - wrong exception type: " << e.what() << "\n";
    }

    sycl::free(in, q);
    sycl::free(out, q);
    return rc;
}

} // namespace

int main()
{
    auto rc {EXIT_SUCCESS};
    rc |= test_happy_path();
    rc |= test_error_path();
    return rc;
}

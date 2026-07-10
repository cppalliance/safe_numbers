//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  Reporting through the device funnel directly must map every
//  exception_type enumerator to the matching std exception on the host
//  (the SYCL analog of the CUDA invalid_argument round-trip test).

#include "sycl_test.hpp"

template <typename Expected>
int run_direct_report(const char* label, const boost::safe_numbers::detail::exception_type exc)
{
    sycl::queue q;
    boost::safe_numbers::device_error_context ctx {q};

    q.single_task([=]()
    {
        boost::safe_numbers::detail::report_device_error(exc, "mapping_test", 1, "direct report mapping");
    });

    try
    {
        ctx.synchronize();
    }
    catch (const Expected& e)
    {
        std::cout << label << ": PASSED (" << e.what() << ")\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << label << ": FAILED - wrong exception type: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    std::cerr << label << ": FAILED - no exception thrown\n";
    return EXIT_FAILURE;
}

int main()
{
    using boost::safe_numbers::detail::exception_type;

    int rc {EXIT_SUCCESS};
    rc |= run_direct_report<std::domain_error>("domain_error mapping", exception_type::domain_error);
    rc |= run_direct_report<std::overflow_error>("overflow_error mapping", exception_type::overflow);
    rc |= run_direct_report<std::underflow_error>("underflow_error mapping", exception_type::underflow);
    rc |= run_direct_report<std::invalid_argument>("invalid_argument mapping", exception_type::invalid_argument);
    rc |= run_direct_report<std::runtime_error>("unknown mapping", exception_type::unknown);
    return rc;
}

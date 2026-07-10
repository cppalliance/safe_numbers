//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  Only one device_error_context may exist at a time (the device buffer is a
//  single device_global shared by all contexts). A second simultaneous
//  context must throw std::logic_error; sequential contexts are fine.

#include "sycl_test.hpp"

int main()
{
    sycl::queue q;

    int rc {EXIT_SUCCESS};

    {
        boost::safe_numbers::device_error_context ctx {q};

        bool caught {false};
        try
        {
            boost::safe_numbers::device_error_context second {q};
        }
        catch (const std::logic_error&)
        {
            caught = true;
        }

        if (!caught)
        {
            std::cerr << "FAILED: second simultaneous context did not throw std::logic_error\n";
            rc = EXIT_FAILURE;
        }
    }

    // After the first context is destroyed a new one may be created
    try
    {
        boost::safe_numbers::device_error_context ctx {q};
        ctx.synchronize();
    }
    catch (const std::exception& e)
    {
        std::cerr << "FAILED: sequential context construction threw: " << e.what() << "\n";
        rc = EXIT_FAILURE;
    }

    std::cout << "single context rule: " << (rc == EXIT_SUCCESS ? "PASSED" : "FAILED") << "\n";
    return rc;
}

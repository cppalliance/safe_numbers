//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Verifies that a std::invalid_argument raised through the device error
// reporter is rethrown on the host as std::invalid_argument (and not folded
// into std::domain_error). This exercises both host-side reporting inside a
// CUDA translation unit and the device-kernel -> synchronize() round-trip.

#include <iostream>
#include <stdexcept>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/literals.hpp>
#include <boost/safe_numbers/cuda_error_reporting.hpp>

#include <cuda_runtime.h>

using namespace boost::safe_numbers::literals;

// Records an invalid_argument on the device, exactly as
// BOOST_SAFE_NUMBERS_THROW_EXCEPTION(std::invalid_argument, ...) does.
__global__ void invalid_argument_kernel()
{
    if (blockDim.x * blockIdx.x + threadIdx.x == 0)
    {
        boost::safe_numbers::detail::report_device_error(
            boost::safe_numbers::detail::to_exception_enum<std::invalid_argument>(),
            __FILE__, __LINE__, "invalid_argument on device");
    }
}

int main()
{
    int failures {0};

    // Host reporting inside a CUDA TU: a bad string literal reports through the
    // host branch of report_device_error and must surface as invalid_argument.
    try
    {
        static_cast<void>(operator""_u128("-1"));
        std::cerr << "host: expected std::invalid_argument, none thrown" << std::endl;
        ++failures;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "host literal: caught std::invalid_argument: " << e.what() << std::endl;
    }

    // Device round-trip: the kernel records invalid_argument and synchronize()
    // must rethrow it as std::invalid_argument.
    boost::safe_numbers::device_error_context ctx;
    invalid_argument_kernel<<<1, 1>>>();
    try
    {
        ctx.synchronize();
        std::cerr << "device: expected std::invalid_argument, none thrown" << std::endl;
        ++failures;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "device kernel: caught std::invalid_argument: " << e.what() << std::endl;
    }

    if (failures == 0)
    {
        std::cerr << "Test PASSED\n";
        return 0;
    }

    return EXIT_FAILURE;
}

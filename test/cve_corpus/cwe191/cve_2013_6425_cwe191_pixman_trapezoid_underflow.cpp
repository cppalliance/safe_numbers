// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2013-6425
// cwe:                CWE-191
// product:            Pixman (as used in X.Org server and cairo)
// version:            before 0.32.0
// summary:            Integer underflow in the pixman_trapezoid_valid macro allows denial of service.
// root-cause:         unsigned subtraction (bottom - top) with bottom less than top
// root-cause-width:   u32
// trigger:            bottom=5, top=10
// consequence:        wrapped height drives an out of bounds access
// classification:     PREVENTED_RUNTIME
// expected-exception: std::underflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2013-6425
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-191&resultsPerPage=200&startIndex=0
// notes:              The trapezoid validity check computes bottom - top; a reversed pair wraps. Modeled at u32.
// BOOST_SAFE_NUMBERS_CVE_END
// ===========================================================================

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE
import boost.safe_numbers;
#else
#include <boost/safe_numbers.hpp>
#endif

#include <cstdint>
#include <stdexcept>

using namespace boost::safe_numbers;

void native_reproduction()
{
    const std::uint32_t bottom {5U};                 // attacker supplied, reversed pair
    const std::uint32_t top    {10U};
    const std::uint32_t height {bottom - top};       // well defined unsigned wrap

    BOOST_TEST_EQ(height, 4294967291U);              // wraps near UINT32_MAX
    BOOST_TEST(height > bottom);
}

void safe_reproduction()
{
    const u32 bottom {5U};
    const u32 top    {10U};

    BOOST_TEST_THROWS((void)(bottom - top), std::underflow_error);
    BOOST_TEST(!checked_sub(bottom, top).has_value());
}

void bounded_reproduction()
{
    // A length required to be at least a header size is declared with a minimum
    // bound; a too small length is rejected before the subtraction underflows.
    using bounded_length = bounded_uint<8u, 1000000u>;
    std::uint32_t attacker_length {4U};
    BOOST_TEST_THROWS((bounded_length{u32{attacker_length}}), std::domain_error);
    BOOST_TEST_NO_THROW((void)bounded_length{u32{64U}});
}

int main()
{
    native_reproduction();
    safe_reproduction();
    bounded_reproduction();
    return boost::report_errors();
}

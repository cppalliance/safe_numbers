// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2016-3623
// cwe:                CWE-369
// product:            LibTIFF (rgb2ycbcr tool)
// version:            4.0.6 and earlier
// summary:            Divide by zero via a zero subsampling value crashes the tool.
// root-cause:         division (dimension / subsampling) with an attacker controlled zero subsampling
// root-cause-width:   u32
// trigger:            dimension=1024, subsampling=0
// consequence:        divide by zero (SIGFPE) denial of service
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2016-3623
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-369&resultsPerPage=200&startIndex=0
// notes:              A zero horizontal or vertical subsampling value reaches a division. Modeled at u32.
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
    const std::uint32_t dimension   {1024U};
    const std::uint32_t subsampling {0U};            // attacker controlled

    BOOST_TEST_EQ(subsampling, 0U);
    BOOST_TEST(dimension > 0U);
}

void safe_reproduction()
{
    const u32 dimension   {1024U};
    const u32 subsampling {0U};

    BOOST_TEST_THROWS((void)(dimension / subsampling), std::domain_error);
    BOOST_TEST(!checked_div(dimension, subsampling).has_value());
}

void bounded_reproduction()
{
    // A divisor required to be non zero is declared with a minimum of one; the
    // zero divisor is rejected before the division can trap.
    using nonzero_divisor = bounded_uint<1u, 1000000000u>;
    std::uint32_t attacker_divisor {0U};
    BOOST_TEST_THROWS((nonzero_divisor{u32{attacker_divisor}}), std::domain_error);
    BOOST_TEST_NO_THROW((void)nonzero_divisor{u32{7U}});
}

int main()
{
    native_reproduction();
    safe_reproduction();
    bounded_reproduction();
    return boost::report_errors();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2004-0184
// cwe:                CWE-191
// product:            tcpdump (isakmp_id_print)
// version:            3.8.1 and earlier
// summary:            Integer underflow in ISAKMP identification printing causes a crash.
// root-cause:         unsigned subtraction (item_len - fixed_header) with item_len too small
// root-cause-width:   u32
// trigger:            item_len=2, fixed_header=4
// consequence:        wrapped length drives an out of bounds read and crash
// classification:     PREVENTED_RUNTIME
// expected-exception: std::underflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2004-0184
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-191&resultsPerPage=200&startIndex=0
// notes:              A short ISAKMP item length is reduced by a fixed header size. Modeled at u32.
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
    const std::uint32_t item_len     {2U};           // attacker supplied, too small
    const std::uint32_t fixed_header {4U};
    const std::uint32_t payload {item_len - fixed_header}; // well defined unsigned wrap

    BOOST_TEST_EQ(payload, 4294967294U);             // wraps near UINT32_MAX
    BOOST_TEST(payload > item_len);
}

void safe_reproduction()
{
    const u32 item_len     {2U};
    const u32 fixed_header {4U};

    BOOST_TEST_THROWS((void)(item_len - fixed_header), std::underflow_error);
    BOOST_TEST(!checked_sub(item_len, fixed_header).has_value());
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

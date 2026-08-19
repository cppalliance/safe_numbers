// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2011-2497
// cwe:                CWE-191
// product:            Linux kernel Bluetooth L2CAP (l2cap_config_req)
// version:            before 3.0
// summary:            Integer underflow in L2CAP config handling allows remote denial of service or worse.
// root-cause:         unsigned subtraction (command_len - option_header) with command_len too small
// root-cause-width:   u32
// trigger:            command_len=4, option_header=8
// consequence:        wrapped remaining length drives an over read or over write
// classification:     PREVENTED_RUNTIME
// expected-exception: std::underflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2011-2497
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-191&resultsPerPage=200&startIndex=0
// notes:              A short command length is decremented by a fixed option header size. Modeled at u32.
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
    const std::uint32_t command_len   {4U};          // attacker supplied, too small
    const std::uint32_t option_header {8U};
    const std::uint32_t remaining {command_len - option_header}; // well defined unsigned wrap

    BOOST_TEST_EQ(remaining, 4294967292U);           // wraps near UINT32_MAX
    BOOST_TEST(remaining > command_len);             // apparent remaining exceeds the packet
}

void safe_reproduction()
{
    const u32 command_len   {4U};
    const u32 option_header {8U};

    BOOST_TEST_THROWS((void)(command_len - option_header), std::underflow_error);
    BOOST_TEST(!checked_sub(command_len, option_header).has_value());
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

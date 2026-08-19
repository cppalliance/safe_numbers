// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2014-9087
// cwe:                CWE-191
// product:            Libksba (as used in GnuPG)
// version:            before 1.3.2
// summary:            Integer underflow in ksba_oid_to_str from a zero length OID triggers a buffer overflow.
// root-cause:         unsigned subtraction (length - 1) with length zero wraps to a huge value
// root-cause-width:   u32
// trigger:            length=0, then length - 1
// consequence:        oversized length drives an out of bounds write
// classification:     PREVENTED_RUNTIME
// expected-exception: std::underflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2014-9087
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-191&resultsPerPage=200&startIndex=0
// notes:              A zero length field flows into a length - 1 computation. Modeled at u32 width;
// notes:              the wrap yields UINT32_MAX.
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
    const std::uint32_t length {0U};                 // attacker supplied zero length
    const std::uint32_t n {length - 1U};             // well defined unsigned wrap

    BOOST_TEST_EQ(n, 4294967295U);                   // wraps to UINT32_MAX
    BOOST_TEST(n > length);                           // apparent size far exceeds the real length
}

void safe_reproduction()
{
    const u32 length {0U};

    BOOST_TEST_THROWS((void)(length - u32{1U}), std::underflow_error);
    BOOST_TEST(!checked_sub(length, u32{1U}).has_value());
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

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2005-0199
// cwe:                CWE-191
// product:            ngIRCd (Lists_MakeMask in lists.c)
// version:            before 0.8.2
// summary:            Integer underflow in mask list handling.
// root-cause:         unsigned subtraction (length - offset) with length too small
// root-cause-width:   u32
// trigger:            length=1, offset=3
// consequence:        crash from an out of bounds access
// classification:     PREVENTED_RUNTIME
// expected-exception: std::underflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2005-0199
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-191&resultsPerPage=200&startIndex=0
// notes:              Lists_MakeMask subtracts an offset from a short length.
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
    const std::uint32_t length {1U};
    const std::uint32_t offset {3U};
    const std::uint32_t remaining {length - offset};

    BOOST_TEST_EQ(remaining, 4294967294U);
    BOOST_TEST(remaining > length);
}

void safe_reproduction()
{
    const u32 length {1U};
    const u32 offset {3U};

    BOOST_TEST_THROWS((void)(length - offset), std::underflow_error);
    BOOST_TEST(!checked_sub(length, offset).has_value());
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

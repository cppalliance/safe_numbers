// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2011-1770
// cwe:                CWE-191
// product:            Linux kernel DCCP (dccp_parse_options)
// version:            before 2.6.33.14
// summary:            Integer underflow in DCCP option parsing.
// root-cause:         unsigned subtraction (option_length - fixed) with option_length too small
// root-cause-width:   u32
// trigger:            option_length=2, fixed=12
// consequence:        remote denial of service
// classification:     PREVENTED_RUNTIME
// expected-exception: std::underflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2011-1770
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-191&resultsPerPage=200&startIndex=0
// notes:              dccp_parse_options reduces a short option length by a fixed amount.
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
    const std::uint32_t option_length {2U};
    const std::uint32_t fixed {12U};
    const std::uint32_t remaining {option_length - fixed};

    BOOST_TEST_EQ(remaining, 4294967286U);
    BOOST_TEST(remaining > option_length);
}

void safe_reproduction()
{
    const u32 option_length {2U};
    const u32 fixed {12U};

    BOOST_TEST_THROWS((void)(option_length - fixed), std::underflow_error);
    BOOST_TEST(!checked_sub(option_length, fixed).has_value());
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

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2010-2497
// cwe:                CWE-191
// product:            FreeType (glyph handling)
// version:            before 2.4.0
// summary:            Integer underflow in glyph handling.
// root-cause:         unsigned subtraction (count - consumed) with count too small
// root-cause-width:   u32
// trigger:            count=3, consumed=10
// consequence:        crash or code execution
// classification:     PREVENTED_RUNTIME
// expected-exception: std::underflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2010-2497
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-191&resultsPerPage=200&startIndex=0
// notes:              Glyph handling subtracts a consumed count from a smaller total.
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
    const std::uint32_t count {3U};
    const std::uint32_t consumed {10U};
    const std::uint32_t remaining {count - consumed};

    BOOST_TEST_EQ(remaining, 4294967289U);
    BOOST_TEST(remaining > count);
}

void safe_reproduction()
{
    const u32 count {3U};
    const u32 consumed {10U};

    BOOST_TEST_THROWS((void)(count - consumed), std::underflow_error);
    BOOST_TEST(!checked_sub(count, consumed).has_value());
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

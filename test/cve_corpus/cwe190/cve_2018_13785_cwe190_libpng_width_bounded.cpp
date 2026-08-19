// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2018-13785
// cwe:                CWE-190, CWE-369
// product:            libpng
// version:            1.6.34
// summary:            Wrong row_factor calculation in png_check_chunk_length overflows, then divides by zero.
// root-cause:         an oversized image width is used without enforcing the documented width limit
// root-cause-width:   u32
// trigger:            width=2147483648 (2^31), far above the PNG user width limit
// consequence:        integer overflow and resultant divide by zero
// classification:     PREVENTED_BOUNDED
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2018-13785
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              NVD tags this CVE both CWE-190 and CWE-369; it is counted under CWE-190.
// notes:              libpng documents a user width limit (default 1000000). Expressing that
// notes:              domain as bounded_uint rejects the oversized width at construction, before
// notes:              any row_factor arithmetic runs. The width is a runtime value, so the check
// notes:              fires at runtime rather than at compile time.
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

// The documented legal domain of a PNG width, per libpng's default user limit.
using png_width = bounded_uint<1U, 1000000U>;

// Native code applies no domain check on the width; the oversized value flows into
// the row_factor arithmetic and misbehaves.
void native_reproduction()
{
    const std::uint32_t max_legal_width {1000000U};
    std::uint32_t attacker_width {2147483648U}; // 2^31, a runtime (non constant) value

    BOOST_TEST(attacker_width > max_legal_width);   // precondition violated, unchecked natively
}

// The precondition lives in the type; the out of domain width is rejected at the
// boundary, before it can reach the overflow or the divide.
void safe_reproduction()
{
    std::uint32_t attacker_width {2147483648U};
    BOOST_TEST_THROWS((png_width{u32{attacker_width}}), std::domain_error);

    std::uint32_t legal_width {800U};
    BOOST_TEST_NO_THROW((void)png_width{u32{legal_width}}); // a width within the domain is accepted
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

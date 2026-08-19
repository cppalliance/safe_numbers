// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2019-19945
// cwe:                CWE-681
// product:            OpenWrt uhttpd
// version:            through 18.06.5 and 19.x through 19.07.0-rc2
// summary:            Integer signedness error leads to out of bounds access.
// root-cause:         a negative signed length is added to an unsigned offset
// root-cause-width:   mixed
// trigger:            signed_length=-8 combined with an unsigned offset
// consequence:        out of bounds heap access
// classification:     PREVENTED_COMPILETIME
// expected-exception: none
// tier-form:          compile-fail
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2019-19945
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              Mixed signedness arithmetic between i32 and u32 is ill-formed for safe types.
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

using namespace boost::safe_numbers;

int main()
{
    const i32 signed_value {-8};
    const u32 unsigned_bound {16U};

    // Ill-formed: a mixed signedness operation between i32 and u32 is rejected,
    // forcing the developer to resolve the signedness that hides the defect.
    const auto combined = signed_value + unsigned_bound;
    return static_cast<int>(combined == u32{0U}) ? 1 : 0;
}

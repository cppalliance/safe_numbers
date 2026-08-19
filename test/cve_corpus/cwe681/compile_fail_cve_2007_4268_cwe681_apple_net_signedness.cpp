// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2007-4268
// cwe:                CWE-681
// product:            Apple Mac OS X Networking
// version:            10.4 through 10.4.10
// summary:            Integer signedness error allows local code execution.
// root-cause:         a negative signed length is compared against an unsigned bound
// root-cause-width:   mixed
// trigger:            signed_length=-1 compared against an unsigned bound
// consequence:        the check is bypassed then memory corruption
// classification:     PREVENTED_COMPILETIME
// expected-exception: none
// tier-form:          compile-fail
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2007-4268
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              Comparing a signed and an unsigned safe type is ill-formed, surfacing the negative length.
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
    const i32 signed_value {-1};
    const u32 unsigned_bound {1024U};

    // Ill-formed: a mixed signedness operation between i32 and u32 is rejected,
    // forcing the developer to resolve the signedness that hides the defect.
    return (signed_value < unsigned_bound) ? 1 : 0;
}

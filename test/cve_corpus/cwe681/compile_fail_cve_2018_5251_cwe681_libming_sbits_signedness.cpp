// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2018-5251
// cwe:                CWE-681
// product:            libming (readSBits)
// version:            0.4.8
// summary:            Incorrect numeric conversion in libming (readSBits).
// root-cause:         a signed value combined with an unsigned value (signed/unsigned confusion)
// root-cause-width:   mixed
// trigger:            signed_value=-4 combined with an unsigned bound
// consequence:        check bypass then memory corruption
// classification:     PREVENTED_COMPILETIME
// expected-exception: none
// tier-form:          compile-fail
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2018-5251
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              Left shift of a negative value (signedness error).
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
    const i32 signed_value {-4};
    const u32 unsigned_bound {16U};

    // Ill-formed: a mixed signedness operation between i32 and u32 is rejected,
    // forcing the developer to resolve the signedness that hides the defect.
    const auto combined = signed_value + unsigned_bound;
    return static_cast<int>(combined == u32{0U}) ? 1 : 0;
}

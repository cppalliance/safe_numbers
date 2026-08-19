// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2009-0231
// cwe:                CWE-681
// product:            Microsoft Windows (T2EMBED.DLL EOT)
// version:            multiple
// summary:            Incorrect numeric conversion in Microsoft Windows (T2EMBED.DLL EOT).
// root-cause:         a signed value combined with an unsigned value (signed/unsigned confusion)
// root-cause-width:   mixed
// trigger:            signed_value=-1 compared with an unsigned bound
// consequence:        check bypass then memory corruption
// classification:     PREVENTED_COMPILETIME
// expected-exception: none
// tier-form:          compile-fail
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2009-0231
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              Signedness error in the EOT font engine.
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

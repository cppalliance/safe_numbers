// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2008-1721
// cwe:                CWE-681
// product:            CPython zlib extension module
// version:            2.5.2 and earlier
// summary:            Integer signedness error allows code execution via a negative signed value.
// root-cause:         a negative signed length is compared against an unsigned bound (signed/unsigned confusion)
// root-cause-width:   mixed
// trigger:            signed_length=-1 compared against an unsigned limit
// consequence:        the check is bypassed then a heap buffer overflow
// classification:     PREVENTED_COMPILETIME
// expected-exception: none
// tier-form:          compile-fail
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2008-1721
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              Comparing a signed and an unsigned safe type is ill-formed, so the developer
// notes:              must resolve the signedness explicitly, which surfaces the negative length.
// BOOST_SAFE_NUMBERS_CVE_END
// ===========================================================================

#include <boost/safe_numbers/detail/config.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE
import boost.safe_numbers;
#else
#include <boost/safe_numbers.hpp>
#endif

#include <cstdint>

using namespace boost::safe_numbers;

int main()
{
    const i32 signed_length {-1};       // a negative length produced by the signedness bug
    const u32 declared_limit {1024U};

    // Ill-formed: a mixed signedness comparison between i32 and u32 is rejected.
    // In native C the negative length converts to a huge unsigned and passes the check.
    return (signed_length < declared_limit) ? 1 : 0;
}

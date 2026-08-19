// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2016-3074
// cwe:                CWE-681
// product:            GD Graphics Library (libgd)
// version:            2.1.1
// summary:            Integer signedness error allows denial of service or code execution.
// root-cause:         a signed size is added to an unsigned offset (signed/unsigned confusion)
// root-cause-width:   mixed
// trigger:            a negative signed chunk size combined with an unsigned offset
// consequence:        out of bounds access from the mishandled signed value
// classification:     PREVENTED_COMPILETIME
// expected-exception: none
// tier-form:          compile-fail
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2016-3074
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              Mixed signedness arithmetic between i32 and u32 is ill-formed, forcing the
// notes:              developer to resolve the signedness rather than let the implicit rule hide it.
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
    const i32 chunk_size {-4};           // a negative size produced by the signedness bug
    const u32 base_offset {16U};

    // Ill-formed: mixed signedness addition between i32 and u32 is rejected.
    const auto next = chunk_size + base_offset;
    return static_cast<int>(next == u32{12U}) ? 1 : 0;
}

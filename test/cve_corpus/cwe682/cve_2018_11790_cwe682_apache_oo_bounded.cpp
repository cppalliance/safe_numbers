// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2018-11790
// cwe:                CWE-682
// product:            Apache OpenOffice
// version:            4.1.5 and earlier
// summary:            An incorrect line-termination length yields an out of bounds read.
// root-cause:         a computed offset exceeds a line buffer of known size
// root-cause-width:   u8
// trigger:            buffer_size=200 (valid offsets 0..199), computed offset base=200 + extra=10 = 210
// consequence:        out of bounds read
// classification:     PREVENTED_BOUNDED
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2018-11790
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-682&resultsPerPage=200&startIndex=0
// notes:              The addition is correct arithmetic and does not overflow, so the default check
// notes:              does not fire. The defect is that the offset lands past a known line buffer.
// notes:              Bounding the offset by the buffer size (bounded_uint<0, 199>) rejects it.
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

// The line buffer holds 200 bytes, so the valid offset domain is 0 through 199.
using line_offset = bounded_uint<0U, 199U>;

void native_reproduction()
{
    const std::uint32_t buffer_size {200U};
    const std::uint32_t base  {200U};
    const std::uint32_t extra {10U};
    const std::uint32_t offset {base + extra};   // 210, no overflow

    BOOST_TEST_EQ(offset, 210U);
    BOOST_TEST(offset >= buffer_size);           // past the end of the line buffer
}

void safe_reproduction()
{
    const u8 base  {200U};
    const u8 extra {10U};

    // The arithmetic is correct and does not overflow: the default check stays silent.
    BOOST_TEST_NO_THROW((void)(base + extra));
    const u8 offset {base + extra};              // u8{210}

    // Bounding the offset by the known buffer size rejects the out of domain value.
    BOOST_TEST_THROWS((line_offset{offset}), std::domain_error);
    BOOST_TEST_NO_THROW((void)line_offset{u8{199U}}); // the last valid offset is accepted
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

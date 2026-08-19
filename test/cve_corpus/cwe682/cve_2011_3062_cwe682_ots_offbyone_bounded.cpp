// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2011-3062
// cwe:                CWE-682
// product:            OpenType Sanitizer (as used in Google Chrome)
// version:            before 18.0.1025.142
// summary:            Off by one error in the OpenType Sanitizer allows an out of bounds access.
// root-cause:         an index is computed one too large for a buffer of known size
// root-cause-width:   u8
// trigger:            buffer_size=150 (valid indices 0..149), computed index base=100 + length=50 = 150
// consequence:        one element out of bounds access
// classification:     PREVENTED_BOUNDED
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2011-3062
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-682&resultsPerPage=200&startIndex=0
// notes:              The addition 100 + 50 is correct arithmetic and does not overflow, so the
// notes:              default overflow check does not fire. The defect is that the result is used as
// notes:              an index into a 150-element buffer. Declaring the index domain from the known
// notes:              buffer size (bounded_uint<0, 149>) rejects the off-by-one value at the point it
// notes:              is formed as an index.
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

// The known buffer holds 150 elements, so the valid index domain is 0 through 149.
using table_index = bounded_uint<0U, 149U>;

// Native code computes an inclusive end index and uses it directly; the addition is
// correct arithmetic, but the value is one past the last valid index.
void native_reproduction()
{
    const std::uint32_t buffer_size {150U};
    const std::uint32_t base   {100U};
    const std::uint32_t length {50U};
    const std::uint32_t end_index {base + length};   // 150, no overflow

    BOOST_TEST_EQ(end_index, 150U);
    BOOST_TEST(end_index >= buffer_size);            // one past the last valid index (149)
}

// The default arithmetic types do not catch this (the sum is representable), but expressing
// the index domain with a bounded type rejects the out of domain index.
void safe_reproduction()
{
    const u8 base   {100U};
    const u8 length {50U};

    // The arithmetic is correct and does not overflow: the default check stays silent.
    BOOST_TEST_NO_THROW((void)(base + length));
    const u8 end_index {base + length};              // u8{150}

    // Materializing the value as a table index enforces the buffer's domain.
    BOOST_TEST_THROWS((table_index{end_index}), std::domain_error);
    BOOST_TEST_NO_THROW((void)table_index{u8{149U}}); // the last valid index is accepted
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

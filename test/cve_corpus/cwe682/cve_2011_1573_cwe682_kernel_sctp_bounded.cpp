// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2011-1573
// cwe:                CWE-682
// product:            Linux kernel SCTP (sm_make_chunk.c)
// version:            before 2.6.34
// summary:            Incorrect length accounting yields a length larger than the available space.
// root-cause:         a remaining length omits a term, exceeding the known available buffer
// root-cause-width:   u8
// trigger:            available=20 bytes, remaining computed as total=64 - used=40 = 24 (padding omitted)
// consequence:        an over long length drives an out of bounds read or write
// classification:     PREVENTED_BOUNDED
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2011-1573
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-682&resultsPerPage=200&startIndex=0
// notes:              The subtraction 64 - 40 is correct arithmetic and does not underflow, so the
// notes:              default check does not fire. The defect is that the result exceeds the real
// notes:              available space. Bounding the length by the known available size (bounded_uint
// notes:              <0, 20>) rejects the over long value before it is used.
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

// The real available space is 20 bytes, so a valid length is 0 through 20.
using chunk_length = bounded_uint<0U, 20U>;

void native_reproduction()
{
    const std::uint32_t available {20U};
    const std::uint32_t total {64U};
    const std::uint32_t used  {40U};             // omits the padding term (the bug)
    const std::uint32_t remaining {total - used}; // 24, no underflow

    BOOST_TEST_EQ(remaining, 24U);
    BOOST_TEST(remaining > available);           // claims more space than exists
}

void safe_reproduction()
{
    const u8 total {64U};
    const u8 used  {40U};

    // The subtraction is correct and does not underflow: the default check stays silent.
    BOOST_TEST_NO_THROW((void)(total - used));
    const u8 remaining {total - used};           // u8{24}

    // Bounding the length by the known available space rejects the over long value.
    BOOST_TEST_THROWS((chunk_length{remaining}), std::domain_error);
    BOOST_TEST_NO_THROW((void)chunk_length{u8{20U}}); // a length within the buffer is accepted
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

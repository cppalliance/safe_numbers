// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2018-20999
// cwe:                CWE-682
// product:            orion crate (Rust)
// version:            before 0.11.2
// summary:            Incorrect reset() calculation in the orion crate.
// root-cause:         an index computed one too large; the arithmetic stays in range
// root-cause-width:   u32
// trigger:            base=100, length=50
// consequence:        one element out of bounds that the library cannot detect
// classification:     NOT_PREVENTED
// expected-exception: none
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2018-20999
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-682&resultsPerPage=200&startIndex=0
// notes:              In-range calculation error the library cannot detect.
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

void native_reproduction()
{
    const std::uint32_t base {100U};
    const std::uint32_t length {50U};
    const std::uint32_t end_index {base + length};
    const std::uint32_t correct_index {base + length - 1U};

    BOOST_TEST_EQ(end_index, 150U);
    BOOST_TEST(end_index != correct_index);
}

void safe_reproduction()
{
    const u32 base {100U};
    const u32 length {50U};

    BOOST_TEST_NO_THROW((void)(base + length));
    const u32 end_index {base + length};
    BOOST_TEST_EQ(static_cast<std::uint32_t>(end_index), 150U);
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

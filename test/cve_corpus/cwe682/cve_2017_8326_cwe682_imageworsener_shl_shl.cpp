// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2017-8326
// cwe:                CWE-682
// product:            ImageWorsener
// version:            before 1.3.1
// summary:            Left shift overflow in ImageWorsener.
// root-cause:         left shift whose result exceeds the type width
// root-cause-width:   u32
// trigger:            value=16777215, shift=16
// consequence:        incorrect shifted value then memory corruption
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2017-8326
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-682&resultsPerPage=200&startIndex=0
// notes:              A left shift result exceeds the type width.
// notes:              Modeled at the canonical shift overflow pattern for this CWE at the disclosed width.
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

void native_reproduction()
{
    const std::uint32_t value {16777215U};
    const std::uint32_t shift {16U};

    const std::uint64_t true_result {static_cast<std::uint64_t>(value) << shift};
    const std::uint32_t result {static_cast<std::uint32_t>(value << shift)};

    BOOST_TEST_EQ(result, 4294901760U);
    BOOST_TEST(static_cast<std::uint64_t>(result) != true_result);
}

void safe_reproduction()
{
    const u32 value {16777215U};
    const u32 shift {16U};

    BOOST_TEST_THROWS((void)(value << shift), std::overflow_error);
}

int main()
{
    native_reproduction();
    safe_reproduction();
    return boost::report_errors();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2021-21860
// cwe:                CWE-681
// product:            MPEG-4 decoder
// version:            see advisory
// summary:            Incorrect numeric conversion in MPEG-4 decoder.
// root-cause:         a wider value stored into a narrower field, truncating it
// root-cause-width:   mixed
// trigger:            value=80000 into a 16-bit field
// consequence:        undersized field then buffer overflow
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2021-21860
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              A 32-bit value is truncated to a 16-bit field.
// notes:              Modeled at the canonical truncation pattern for this CWE at the disclosed width.
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
    const std::uint32_t value {80000U};
    const std::uint16_t field {static_cast<std::uint16_t>(value)};

    BOOST_TEST_EQ(field, static_cast<std::uint16_t>(14464U));
    BOOST_TEST(static_cast<std::uint32_t>(field) != value);
    BOOST_TEST(static_cast<std::uint32_t>(field) < value);
}

void safe_reproduction()
{
    const u32 value {80000U};

    BOOST_TEST_THROWS((void)static_cast<std::uint16_t>(value), std::domain_error);
}

void bounded_reproduction()
{
    // Declaring the input with its documented maximum rejects the oversized
    // value at construction, before the operation can misbehave.
    using bounded_input = bounded_uint<0u, 1000000u>;
    std::uint32_t attacker_input {2000000000U};
    BOOST_TEST_THROWS((bounded_input{u32{attacker_input}}), std::domain_error);
    BOOST_TEST_NO_THROW((void)bounded_input{u32{1000U}});
}

int main()
{
    native_reproduction();
    safe_reproduction();
    bounded_reproduction();
    return boost::report_errors();
}

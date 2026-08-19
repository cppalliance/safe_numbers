// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2019-10624
// cwe:                CWE-681
// product:            Qualcomm vendor command handler
// version:            see advisory
// summary:            Integer truncation copying an int length into a u8 field yields a buffer overflow.
// root-cause:         a wide length is stored into an 8-bit field, truncating the value
// root-cause-width:   mixed
// trigger:            length=300 stored into an 8-bit field (truncates to 44)
// consequence:        undersized field then buffer overflow
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2019-10624
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              Compute at matched width; the checked narrowing to an 8-bit field throws.
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
    const std::uint32_t length {300U};
    const std::uint8_t field {static_cast<std::uint8_t>(length)};  // silent truncation

    BOOST_TEST_EQ(field, static_cast<std::uint8_t>(44U));          // 300 mod 256
    BOOST_TEST(static_cast<std::uint32_t>(field) != length);
    BOOST_TEST(static_cast<std::uint32_t>(field) < length);
}

void safe_reproduction()
{
    const u32 length {300U};

    BOOST_TEST_THROWS((void)static_cast<std::uint8_t>(length), std::domain_error);
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

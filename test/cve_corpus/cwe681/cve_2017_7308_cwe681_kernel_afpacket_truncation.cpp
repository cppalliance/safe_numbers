// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2017-7308
// cwe:                CWE-681
// product:            Linux kernel (packet_set_ring in af_packet.c)
// version:            through 4.10.6
// summary:            Truncated block-size validation leads to a heap overflow.
// root-cause:         a 32-bit block size is stored into a 16-bit field, truncating the value
// root-cause-width:   mixed
// trigger:            block_size=70000 stored into a 16-bit field
// consequence:        undersized ring block then heap buffer overflow
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2017-7308
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              The block-size field is narrowed; modeled as a 32-bit to 16-bit narrowing.
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
    const std::uint32_t value {70000U};
    const std::uint16_t field {static_cast<std::uint16_t>(value)};

    BOOST_TEST_EQ(field, static_cast<std::uint16_t>(4464U));
    BOOST_TEST(static_cast<std::uint32_t>(field) != value);
    BOOST_TEST(static_cast<std::uint32_t>(field) < value);
}

void safe_reproduction()
{
    const u32 value {70000U};

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

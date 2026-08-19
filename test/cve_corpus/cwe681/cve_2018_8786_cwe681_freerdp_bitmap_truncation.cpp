// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2018-8786
// cwe:                CWE-681
// product:            FreeRDP (update_read_bitmap_update)
// version:            before 2.0.0-rc4
// summary:            Integer truncation leads to a heap based buffer overflow in bitmap update parsing.
// root-cause:         a 32-bit rectangle count is stored into a 16-bit field, truncating the value
// root-cause-width:   mixed
// trigger:            count=70000 stored into a 16-bit field (truncates to 4464)
// consequence:        undersized allocation from the truncated count then heap buffer overflow
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2018-8786
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              Compute at matched width; the checked narrowing to a 16-bit field throws.
// notes:              The conversion check throws regardless of the type error policy.
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

// Native code stores a 32-bit count into a 16-bit field, silently truncating it.
void native_reproduction()
{
    const std::uint32_t count {70000U};
    const std::uint16_t field {static_cast<std::uint16_t>(count)}; // silent truncation

    BOOST_TEST_EQ(field, static_cast<std::uint16_t>(4464U));       // 70000 mod 65536
    BOOST_TEST(static_cast<std::uint32_t>(field) != count);        // lost the high bits
    BOOST_TEST(static_cast<std::uint32_t>(field) < count);         // undercount
}

// The identical narrowing in safe types throws instead of silently truncating.
void safe_reproduction()
{
    const u32 count {70000U};

    BOOST_TEST_THROWS((void)static_cast<std::uint16_t>(count), std::domain_error);
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

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2008-3282
// cwe:                CWE-681
// product:            OpenOffice.org (rtl_allocateMemory, 64-bit)
// version:            2.4.1
// summary:            Integer overflow and truncation in the memory allocator on 64-bit.
// root-cause:         a 64-bit allocation size is truncated when stored into a 32-bit field
// root-cause-width:   mixed
// trigger:            size=4294967300 stored into a 32-bit field
// consequence:        undersized allocation then heap overflow
// classification:     PREVENTED_RUNTIME
// expected-exception: std::domain_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2008-3282
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-681&resultsPerPage=200&startIndex=0
// notes:              A 64-bit size is narrowed to a 32-bit field. Modeled as u64 to u32.
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
    const std::uint64_t value {4294967300ULL};
    const std::uint32_t field {static_cast<std::uint32_t>(value)};

    BOOST_TEST_EQ(field, 4U);
    BOOST_TEST(static_cast<std::uint64_t>(field) != value);
    BOOST_TEST(static_cast<std::uint64_t>(field) < value);
}

void safe_reproduction()
{
    const u64 value {4294967300ULL};

    BOOST_TEST_THROWS((void)static_cast<std::uint32_t>(value), std::domain_error);
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

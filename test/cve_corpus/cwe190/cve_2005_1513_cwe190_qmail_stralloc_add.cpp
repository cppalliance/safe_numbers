// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2005-1513
// cwe:                CWE-190
// product:            qmail (stralloc_readyplus, 64-bit)
// version:            qmail on 64-bit
// summary:            Integer overflow in qmail (stralloc_readyplus, 64-bit).
// root-cause:         unsigned addition of two values
// root-cause-width:   u32
// trigger:            first=4000000000, second=500000000
// consequence:        undersized allocation then memory corruption
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2005-1513
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              Buffer length plus growth overflows on 64-bit.
// notes:              Modeled at the canonical addition overflow pattern for this CWE at the disclosed width.
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
#include "../cve_corpus_util.hpp"

using namespace boost::safe_numbers;

void native_reproduction()
{
    const std::uint32_t first {4000000000U};
    const std::uint32_t second {500000000U};

    const std::uint64_t true_total {cve_corpus::true_add(first, second)};
    const std::uint32_t total {static_cast<std::uint32_t>(first + second)};

    BOOST_TEST(true_total > static_cast<std::uint64_t>(first));
    BOOST_TEST_EQ(total, 205032704U);
    BOOST_TEST(static_cast<std::uint64_t>(total) < true_total);
}

void safe_reproduction()
{
    const u32 first {4000000000U};
    const u32 second {500000000U};

    BOOST_TEST_THROWS((void)(first + second), std::overflow_error);
    BOOST_TEST(!checked_add(first, second).has_value());
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

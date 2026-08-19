// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2014-4608
// cwe:                CWE-190
// product:            Linux kernel LZO decompressor (lzo1x_decompress_safe)
// version:            before 3.15.2
// summary:            Integer overflow in the LZO decompressor length arithmetic causes memory corruption.
// root-cause:         unsigned addition (output_position + run_length) overflows the 32-bit accumulator
// root-cause-width:   u32
// trigger:            output_position=4000000000, run_length=1000000000 (sum exceeds UINT32_MAX)
// consequence:        wrapped length bypasses the safe-decompress bound then buffer overflow
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2014-4608
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              A crafted stream drives a length accumulation past UINT32_MAX; modeled as
// notes:              u32 addition of a position and a run length.
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
    const std::uint32_t output_position {4000000000U};
    const std::uint32_t run_length      {1000000000U};

    const std::uint64_t true_end {cve_corpus::true_add(output_position, run_length)}; // 5000000000
    const std::uint32_t end {static_cast<std::uint32_t>(output_position + run_length)}; // wraps

    BOOST_TEST(true_end > static_cast<std::uint64_t>(output_position));
    BOOST_TEST_EQ(end, 705032704U);                                 // deterministic wrap
    BOOST_TEST(static_cast<std::uint64_t>(end) < true_end);         // apparent end before real end
}

void safe_reproduction()
{
    const u32 output_position {4000000000U};
    const u32 run_length      {1000000000U};

    BOOST_TEST_THROWS((void)(output_position + run_length), std::overflow_error);
    BOOST_TEST(!checked_add(output_position, run_length).has_value());
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

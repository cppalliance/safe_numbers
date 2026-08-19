// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2008-2663
// cwe:                CWE-190
// product:            Ruby (rb_ary_store)
// version:            1.8.x before the fixed patchlevels
// summary:            Integer overflow in Ruby array storage sizing.
// root-cause:         unsigned multiplication (capacity * element_size) for an array buffer
// root-cause-width:   u32
// trigger:            capacity=600000000, element_size=8
// consequence:        undersized array buffer then memory corruption
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2008-2663
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              rb_ary_store grows an array by capacity * sizeof(VALUE) at u32 width.
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
    const std::uint32_t capacity {600000000U};
    const std::uint32_t element_size {8U};

    const std::uint64_t true_bytes {cve_corpus::true_mul(capacity, element_size)};
    const std::uint32_t bytes {static_cast<std::uint32_t>(capacity * element_size)};

    BOOST_TEST(true_bytes > static_cast<std::uint64_t>(capacity));
    BOOST_TEST_EQ(bytes, 505032704U);
    BOOST_TEST(static_cast<std::uint64_t>(bytes) < true_bytes);
}

void safe_reproduction()
{
    const u32 capacity {600000000U};
    const u32 element_size {8U};

    BOOST_TEST_THROWS((void)(capacity * element_size), std::overflow_error);
    BOOST_TEST(!checked_mul(capacity, element_size).has_value());
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

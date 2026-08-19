// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2002-0639
// cwe:                CWE-190
// product:            OpenSSH
// version:            2.9.9 through 3.3
// summary:            Integer overflow in sshd challenge-response auth allows remote code execution.
// root-cause:         unsigned multiplication (nresp * sizeof(char*)) in the response array size
// root-cause-width:   u32
// trigger:            nresp=1073741824, element_size=4 (pointer size on a 32-bit target)
// consequence:        heap buffer overflow (xmalloc(0) then writes nresp pointers)
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2002-0639
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              Attacker sends a large nresp; response = xmalloc(nresp * sizeof(char*))
// notes:              wraps to 0 on a 32-bit target, then the loop writes nresp pointers.
// notes:              Modeled at u32 width; the wrapped product is exactly 0.
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

// Native fixed-width types silently wrap: the response-array byte count wraps to
// zero, xmalloc(0) under-allocates, and the subsequent loop writes nresp pointers
// past the end of the buffer.
void native_reproduction()
{
    const std::uint32_t nresp        {1073741824U}; // 2^30, attacker controlled
    const std::uint32_t element_size {4U};          // sizeof(char*) on a 32-bit target

    // Ground truth in a wider type: 2^30 * 4 == 2^32 == 4294967296.
    const std::uint64_t true_total {cve_corpus::true_mul(nresp, element_size)};

    // The vulnerable 32-bit computation. The explicit cast documents the intended
    // 32-bit wraparound and keeps the line clean under -Wconversion.
    const std::uint32_t total_bytes {static_cast<std::uint32_t>(nresp * element_size)};

    BOOST_TEST(true_total > static_cast<std::uint64_t>(nresp));        // real size exceeds the count
    BOOST_TEST_EQ(total_bytes, 0U);                                    // native product wraps to zero
    BOOST_TEST(static_cast<std::uint64_t>(total_bytes) != true_total); // silently wrong value
    BOOST_TEST(total_bytes < nresp);                                   // buffer smaller than element count
}

// The identical expression in safe types throws at the faulting multiply, before
// any allocation can be made from the wrong size.
void safe_reproduction()
{
    const u32 nresp        {1073741824U};
    const u32 element_size {4U};

    BOOST_TEST_THROWS((void)(nresp * element_size), std::overflow_error);

    // Non-throwing oracle arm (noexcept): checked_mul returns nullopt on the fault,
    // so a caller can branch on the error instead of catching.
    BOOST_TEST(!checked_mul(nresp, element_size).has_value());
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

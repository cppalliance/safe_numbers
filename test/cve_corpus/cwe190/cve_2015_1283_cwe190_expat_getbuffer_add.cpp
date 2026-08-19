// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2015-1283
// cwe:                CWE-190
// product:            Expat (libexpat)
// version:            through 2.1.0
// summary:            Integer overflow in XML_GetBuffer buffer size computation allows heap corruption.
// root-cause:         signed addition (len + keep) of two int buffer sizes overflows INT_MAX
// root-cause-width:   i32
// trigger:            len=2000000000, keep=2000000000 (sum 4000000000 exceeds INT_MAX)
// consequence:        negative/undersized buffer size then heap buffer overflow
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2015-1283
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              XML_GetBuffer computed neededSize = len + keep as int; the fix added
// notes:              the check len > INT_MAX - keep. Modeled as i32 signed addition; the
// notes:              native wrap is shown via the well defined unsigned round trip (no UB).
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
#include <limits>
#include <stdexcept>

using namespace boost::safe_numbers;

// Native signed addition overflows INT_MAX and wraps to a negative value, which is
// then used as an allocation size.
void native_reproduction()
{
    const std::int32_t len  {2000000000};
    const std::int32_t keep {2000000000};

    // Ground truth in a wider signed type: 4000000000 > INT32_MAX.
    const std::int64_t true_size {static_cast<std::int64_t>(len) + static_cast<std::int64_t>(keep)};

    // The vulnerable 32-bit computation, shown via the well defined two's complement
    // round trip so this reproduction itself performs no signed overflow (no UB).
    const std::int32_t needed {static_cast<std::int32_t>(
        static_cast<std::uint32_t>(len) + static_cast<std::uint32_t>(keep))};

    BOOST_TEST(true_size > static_cast<std::int64_t>(std::numeric_limits<std::int32_t>::max()));
    BOOST_TEST(needed < 0);                                          // wraps negative
    BOOST_TEST(static_cast<std::int64_t>(needed) != true_size);      // silently wrong value
}

// The identical addition in safe types throws before the wrong size is produced.
void safe_reproduction()
{
    const i32 len  {2000000000};
    const i32 keep {2000000000};

    BOOST_TEST_THROWS((void)(len + keep), std::overflow_error);
    BOOST_TEST(!checked_add(len, keep).has_value());
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

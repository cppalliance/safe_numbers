// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2009-0946
// cwe:                CWE-190
// product:            FreeType (smooth rasterizer)
// version:            2.3.9 and earlier
// summary:            Integer overflow from large font input values.
// root-cause:         unsigned multiplication (width * rows) of glyph bitmap dimensions
// root-cause-width:   u32
// trigger:            width=70000, rows=70000
// consequence:        undersized bitmap then code execution
// classification:     PREVENTED_RUNTIME
// expected-exception: std::overflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2009-0946
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-190&resultsPerPage=200&startIndex=0
// notes:              The smooth rasterizer sizes a glyph bitmap from width * rows at u32 width.
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
    const std::uint32_t width {70000U};
    const std::uint32_t rows {70000U};

    const std::uint64_t true_pixels {cve_corpus::true_mul(width, rows)};
    const std::uint32_t pixels {static_cast<std::uint32_t>(width * rows)};

    BOOST_TEST(true_pixels > static_cast<std::uint64_t>(width));
    BOOST_TEST_EQ(pixels, 605032704U);
    BOOST_TEST(static_cast<std::uint64_t>(pixels) < true_pixels);
}

void safe_reproduction()
{
    const u32 width {70000U};
    const u32 rows {70000U};

    BOOST_TEST_THROWS((void)(width * rows), std::overflow_error);
    BOOST_TEST(!checked_mul(width, rows).has_value());
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

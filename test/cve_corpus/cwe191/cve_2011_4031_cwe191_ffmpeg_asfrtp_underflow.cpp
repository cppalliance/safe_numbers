// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===========================================================================
// BOOST_SAFE_NUMBERS_CVE_BEGIN
// cve-id:             CVE-2011-4031
// cwe:                CWE-191
// product:            FFmpeg (asfrtp_parse_packet)
// version:            before 0.8.3
// summary:            Integer underflow in ASF RTP depacketization allows code execution.
// root-cause:         unsigned subtraction (packet_size - header) with packet_size too small
// root-cause-width:   u32
// trigger:            packet_size=4, header=20
// consequence:        code execution
// classification:     PREVENTED_RUNTIME
// expected-exception: std::underflow_error
// tier-form:          run
// reference:          https://nvd.nist.gov/vuln/detail/CVE-2011-4031
// nvd-cwe-source:     primary
// sampled-on:         2026-08-19
// sample-query:       cweId=CWE-191&resultsPerPage=200&startIndex=0
// notes:              asfrtp_parse_packet subtracts a header size from a short packet size.
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
    const std::uint32_t packet_size {4U};
    const std::uint32_t header {20U};
    const std::uint32_t remaining {packet_size - header};

    BOOST_TEST_EQ(remaining, 4294967280U);
    BOOST_TEST(remaining > packet_size);
}

void safe_reproduction()
{
    const u32 packet_size {4U};
    const u32 header {20U};

    BOOST_TEST_THROWS((void)(packet_size - header), std::underflow_error);
    BOOST_TEST(!checked_sub(packet_size, header).has_value());
}

void bounded_reproduction()
{
    // A length required to be at least a header size is declared with a minimum
    // bound; a too small length is rejected before the subtraction underflows.
    using bounded_length = bounded_uint<8u, 1000000u>;
    std::uint32_t attacker_length {4U};
    BOOST_TEST_THROWS((bounded_length{u32{attacker_length}}), std::domain_error);
    BOOST_TEST_NO_THROW((void)bounded_length{u32{64U}});
}

int main()
{
    native_reproduction();
    safe_reproduction();
    bounded_reproduction();
    return boost::report_errors();
}

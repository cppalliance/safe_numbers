// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Semantic probes for Boost.SafeNumbers. See run_probes.py for the probe list.

#include "probe_common.hpp"
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/bounded_integers.hpp>
#include <cstdint>
#include <climits>

using namespace boost::safe_numbers;

int main()
{
    // volatile sources keep every operand a runtime value
    volatile std::uint8_t v8_255 {255U};
    volatile std::uint8_t v8_1 {1U};
    volatile std::uint32_t v32_0 {0U};
    volatile std::uint32_t v32_1 {1U};
    volatile std::uint32_t v32_2 {2U};
    volatile std::uint32_t v32_half {0x80000000U};
    volatile std::uint32_t v32_300 {300U};
    volatile std::int32_t vs_min {INT_MIN};
    volatile std::int32_t vs_m1 {-1};

    const std::uint8_t r8_255 {v8_255};
    const std::uint8_t r8_1 {v8_1};
    const std::uint32_t r32_0 {v32_0};
    const std::uint32_t r32_1 {v32_1};
    const std::uint32_t r32_2 {v32_2};
    const std::uint32_t r32_half {v32_half};
    const std::uint32_t r32_300 {v32_300};
    const std::int32_t rs_min {vs_min};
    const std::int32_t rs_m1 {vs_m1};

    (void)r8_255; (void)r8_1; (void)r32_0; (void)r32_1; (void)r32_2;
    (void)r32_half; (void)r32_300; (void)rs_min; (void)rs_m1;

    return run_probe([&]()
    {
#if PROBE == 1
        // narrow overflow: u8 255 + 1
        const u8 a {r8_255};
        const u8 b {r8_1};
        print_value(static_cast<std::uint8_t>(a + b));
#elif PROBE == 2
        // unsigned underflow: u32 0 - 1
        const u32 a {r32_0};
        const u32 b {r32_1};
        print_value(static_cast<std::uint32_t>(a - b));
#elif PROBE == 3
        // overflow at the declared width: u32 2^31 * 2
        const u32 a {r32_half};
        const u32 b {r32_2};
        print_value(static_cast<std::uint32_t>(a * b));
#elif PROBE == 4
        // signed asymmetric case: INT_MIN / -1
        const i32 a {rs_min};
        const i32 b {rs_m1};
        print_value(static_cast<std::int32_t>(a / b));
#elif PROBE == 5
        // division by a runtime zero
        const u32 a {r32_1};
        const u32 z {r32_0};
        print_value(static_cast<std::uint32_t>(a / z));
#elif PROBE == 6
        // mixed signedness comparison: -1 < 1u
        const i32 a {rs_m1};
        const u32 b {r32_1};
        print_value(static_cast<int>(a < b));
#elif PROBE == 7
        // truncating conversion: u32 300 -> u8 (explicit narrowing cast to the narrow width)
        const u32 a {r32_300};
        const auto narrow {static_cast<std::uint8_t>(a)};
        print_value(narrow);
#elif PROBE == 8
        // mixed width arithmetic: u8 + u32
        const u8 a {r8_1};
        const u32 b {r32_1};
        print_value(static_cast<std::uint32_t>(a + b));
#elif PROBE == 9
        // construction from bool
        volatile bool vb {true};
        const bool rb {vb};
        const u8 a {rb};
        print_value(static_cast<std::uint8_t>(a));
#elif PROBE == 10
        // bounded domain [0, 100]: 60 + 50
        volatile std::uint32_t v60 {60U};
        volatile std::uint32_t v50 {50U};
        const std::uint32_t r60 {v60};
        const std::uint32_t r50 {v50};
        using pct = bounded_uint<0U, 100U>;
        const pct a {static_cast<std::uint8_t>(r60)};
        const pct b {static_cast<std::uint8_t>(r50)};
        print_value(static_cast<std::uint8_t>(static_cast<u8>(a + b)));
#endif
    });
}

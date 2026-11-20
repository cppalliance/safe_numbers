// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Semantic probes for Boost.SafeNumerics in its default configuration
// (native promotion policy, default exception policy).

#include "probe_common.hpp"
#include <boost/safe_numerics/safe_integer.hpp>
#include <boost/safe_numerics/safe_integer_range.hpp>
#include <cstdint>
#include <climits>

using boost::safe_numerics::safe;
using boost::safe_numerics::safe_unsigned_range;

int main()
{
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
        const safe<std::uint8_t> a {r8_255};
        const safe<std::uint8_t> b {r8_1};
        const auto r {a + b};
        print_value(static_cast<long long>(r));
#elif PROBE == 2
        const safe<std::uint32_t> a {r32_0};
        const safe<std::uint32_t> b {r32_1};
        const auto r {a - b};
        print_value(static_cast<long long>(r));
#elif PROBE == 3
        const safe<std::uint32_t> a {r32_half};
        const safe<std::uint32_t> b {r32_2};
        const auto r {a * b};
        print_value(static_cast<long long>(r));
#elif PROBE == 4
        const safe<std::int32_t> a {rs_min};
        const safe<std::int32_t> b {rs_m1};
        const auto r {a / b};
        print_value(static_cast<long long>(r));
#elif PROBE == 5
        const safe<std::uint32_t> a {r32_1};
        const safe<std::uint32_t> z {r32_0};
        const auto r {a / z};
        print_value(static_cast<long long>(r));
#elif PROBE == 6
        const safe<std::int32_t> a {rs_m1};
        const safe<std::uint32_t> b {r32_1};
        print_value(static_cast<int>(a < b));
#elif PROBE == 7
        const safe<std::uint32_t> a {r32_300};
        const safe<std::uint8_t> r {a};
        print_value(static_cast<long long>(r));
#elif PROBE == 8
        const safe<std::uint8_t> a {r8_1};
        const safe<std::uint32_t> b {r32_1};
        const auto r {a + b};
        print_value(static_cast<long long>(r));
#elif PROBE == 9
        volatile bool vb {true};
        const bool rb {vb};
        const safe<std::uint8_t> a {rb};
        print_value(static_cast<long long>(a));
#elif PROBE == 10
        volatile std::uint32_t v60 {60U};
        volatile std::uint32_t v50 {50U};
        const std::uint32_t r60 {v60};
        const std::uint32_t r50 {v50};
        const safe_unsigned_range<0, 100> a {r60};
        const safe_unsigned_range<0, 100> b {r50};
        const auto r {a + b};
        print_value(static_cast<long long>(r));
#endif
    });
}

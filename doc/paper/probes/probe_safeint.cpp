// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Semantic probes for Microsoft SafeInt in its default configuration
// (SafeIntException on error). SafeIntException does not derive from
// std::exception, so it is caught here before the common harness.

#include "probe_common.hpp"
#include <SafeInt.hpp>
#include <cstdint>
#include <climits>

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
        try
        {
#if PROBE == 1
            const SafeInt<std::uint8_t> a {r8_255};
            const SafeInt<std::uint8_t> b {r8_1};
            const SafeInt<std::uint8_t> r {a + b};
            print_value(static_cast<std::uint8_t>(r));
#elif PROBE == 2
            const SafeInt<std::uint32_t> a {r32_0};
            const SafeInt<std::uint32_t> b {r32_1};
            const SafeInt<std::uint32_t> r {a - b};
            print_value(static_cast<std::uint32_t>(r));
#elif PROBE == 3
            const SafeInt<std::uint32_t> a {r32_half};
            const SafeInt<std::uint32_t> b {r32_2};
            const SafeInt<std::uint32_t> r {a * b};
            print_value(static_cast<std::uint32_t>(r));
#elif PROBE == 4
            const SafeInt<std::int32_t> a {rs_min};
            const SafeInt<std::int32_t> b {rs_m1};
            const SafeInt<std::int32_t> r {a / b};
            print_value(static_cast<std::int32_t>(r));
#elif PROBE == 5
            const SafeInt<std::uint32_t> a {r32_1};
            const SafeInt<std::uint32_t> z {r32_0};
            const SafeInt<std::uint32_t> r {a / z};
            print_value(static_cast<std::uint32_t>(r));
#elif PROBE == 6
            const SafeInt<std::int32_t> a {rs_m1};
            const SafeInt<std::uint32_t> b {r32_1};
            print_value(static_cast<int>(a < b));
#elif PROBE == 7
            const SafeInt<std::uint32_t> a {r32_300};
            const SafeInt<std::uint8_t> r {a};
            print_value(static_cast<std::uint8_t>(r));
#elif PROBE == 8
            const SafeInt<std::uint8_t> a {r8_1};
            const SafeInt<std::uint32_t> b {r32_1};
            const auto r {a + b};
            print_value(static_cast<long long>(r));
#elif PROBE == 9
            volatile bool vb {true};
            const bool rb {vb};
            const SafeInt<std::uint8_t> a {rb};
            print_value(static_cast<std::uint8_t>(a));
#elif PROBE == 10
            // SafeInt has no range-constrained type
            std::printf("NOT_EXPRESSIBLE no bounded type\n");
#endif
        }
        catch (const SafeIntException& e)
        {
            std::printf("THREW SafeIntException code=%d\n", static_cast<int>(e.m_code));
        }
    });
}

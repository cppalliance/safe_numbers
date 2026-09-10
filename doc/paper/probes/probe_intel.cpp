// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Semantic probes for Intel safe-arithmetic. The library proves interval
// containment at compile time; runtime values enter through safe::match, which
// checks them against the declared interval and calls the fallback otherwise.
// Every probe is written the way the library expects: a runtime value is
// admitted through match, and the operation is performed on interval types.

#include "probe_common.hpp"
#include <safe.hpp>
#include <cstdint>
#include <climits>

using namespace safe::interval_types;
using namespace safe::literals;

int main()
{
    volatile std::uint32_t v32_0 {0U};
    volatile std::uint32_t v32_1 {1U};
    volatile std::uint32_t v32_2 {2U};
    volatile std::uint32_t v32_half {0x80000000U};
    volatile std::uint32_t v32_300 {300U};
    volatile std::uint8_t v8_255 {255U};
    volatile std::uint8_t v8_1 {1U};
    volatile std::int32_t vs_min {INT_MIN};
    volatile std::int32_t vs_m1 {-1};

    const std::uint32_t r32_0 {v32_0};
    const std::uint32_t r32_1 {v32_1};
    const std::uint32_t r32_2 {v32_2};
    const std::uint32_t r32_half {v32_half};
    const std::uint32_t r32_300 {v32_300};
    const std::uint8_t r8_255 {v8_255};
    const std::uint8_t r8_1 {v8_1};
    const std::int32_t rs_min {vs_min};
    const std::int32_t rs_m1 {vs_m1};

    (void)r32_0; (void)r32_1; (void)r32_2; (void)r32_half; (void)r32_300;
    (void)r8_255; (void)r8_1; (void)rs_min; (void)rs_m1;

    return run_probe([&]()
    {
#if PROBE == 1
        // u8 255 + 1 stored back into a u8 interval
        auto f = safe::match([](ival_u8<0, 255> a, ival_u8<0, 255> b)
        {
            ival_u8<0, 255> r = a + b;
            return static_cast<unsigned>(r.unsafe_value());
        }, []() { return 9999U; });
        print_value(f(r8_255, r8_1));
#elif PROBE == 2
        // u32 0 - 1 on full-range intervals
        auto f = safe::match([](ival_u32<0, 4294967295> a, ival_u32<0, 4294967295> b)
        {
            auto r = a - b;
            return static_cast<long long>(r.unsafe_value());
        }, []() { return 9999LL; });
        print_value(f(r32_0, r32_1));
#elif PROBE == 3
        // u32 2^31 * 2 on full-range intervals
        auto f = safe::match([](ival_u32<0, 4294967295> a, ival_u32<0, 4294967295> b)
        {
            auto r = a * b;
            return static_cast<long long>(r.unsafe_value());
        }, []() { return 9999LL; });
        print_value(f(r32_half, r32_2));
#elif PROBE == 4
        // INT_MIN / -1 on full-range signed intervals
        auto f = safe::match([](ival_s32<INT_MIN, INT_MAX> a, ival_s32<-1, -1> b)
        {
            auto r = a / b;
            return static_cast<long long>(r.unsafe_value());
        }, []() { return 9999LL; });
        print_value(f(rs_min, rs_m1));
#elif PROBE == 5
        // division by a divisor whose interval includes zero
        auto f = safe::match([](ival_u32<0, 4294967295> a, ival_u32<0, 4294967295> b)
        {
            auto r = a / b;
            return static_cast<long long>(r.unsafe_value());
        }, []() { return 9999LL; });
        print_value(f(r32_1, r32_0));
#elif PROBE == 6
        // mixed signedness comparison
        auto f = safe::match([](ival_s32<-1, -1> a, ival_u32<1, 1> b)
        {
            return static_cast<int>(a < b);
        }, []() { return 9999; });
        print_value(f(rs_m1, r32_1));
#elif PROBE == 7
        // truncating conversion: a [0, 300] interval into a u8 interval
        auto f = safe::match([](ival_u32<0, 300> a)
        {
            ival_u8<0, 255> r = a;
            return static_cast<unsigned>(r.unsafe_value());
        }, []() { return 9999U; });
        print_value(f(r32_300));
#elif PROBE == 8
        // mixed width arithmetic: u8 + u32
        auto f = safe::match([](ival_u8<0, 255> a, ival_u32<0, 255> b)
        {
            auto r = a + b;
            return static_cast<long long>(r.unsafe_value());
        }, []() { return 9999LL; });
        print_value(f(r8_1, r32_1));
#elif PROBE == 9
        // construction from bool
        volatile bool vb {true};
        const bool rb {vb};
        ival_u8<0, 1> a = rb;
        print_value(static_cast<unsigned>(a.unsafe_value()));
#elif PROBE == 10
        // bounded domain [0, 100]: 60 + 50 stored back into the domain
        auto f = safe::match([](ival_u32<0, 100> a, ival_u32<0, 100> b)
        {
            ival_u32<0, 100> r = a + b;
            return r.unsafe_value();
        }, []() { return 9999U; });
        volatile std::uint32_t v60 {60U};
        volatile std::uint32_t v50 {50U};
        const std::uint32_t r60 {v60};
        const std::uint32_t r50 {v50};
        print_value(f(r60, r50));
#endif
    });
}

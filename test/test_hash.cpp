// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <functional>
#include <type_traits>
#include <unordered_set>

#endif

using namespace boost::safe_numbers;

// Each library type is hashable, hashes deterministically, agrees with the
// underlying hash where one exists, and works as an unordered_set key.
template <typename T>
void test_type()
{
    using underlying_type = detail::underlying_type_t<T>;

    const T a {static_cast<underlying_type>(7)};
    const T b {static_cast<underlying_type>(7)};
    const T c {static_cast<underlying_type>(8)};

    const std::hash<T> h {};

    // Deterministic: equal values hash equally
    BOOST_TEST(h(a) == h(b));

    // Matches the underlying built-in hash for every type that has one (all but 128-bit)
    if constexpr (!(std::is_same_v<underlying_type, boost::int128::uint128_t> ||
                    std::is_same_v<underlying_type, boost::int128::int128_t>))
    {
        BOOST_TEST(h(a) == std::hash<underlying_type>{}(static_cast<underlying_type>(a)));
    }

    // Usable as an unordered_set key
    std::unordered_set<T> set {};
    set.insert(a);
    set.insert(c);
    BOOST_TEST(set.count(b) == 1u);
    BOOST_TEST(set.size() == 2u);
}

// Float values, including signed zero, hash identically to the built-in.
void test_float_values()
{
    const std::hash<f64> h64 {};
    for (const double v : {0.0, -0.0, 1.5, -2.5, 3.141592653589793, 1.0e300})
    {
        BOOST_TEST(h64(f64{v}) == std::hash<double>{}(v));
    }

    const std::hash<f32> h32 {};
    for (const float v : {0.0F, -0.0F, 1.5F, -2.5F, 1.0e30F})
    {
        BOOST_TEST(h32(f32{v}) == std::hash<float>{}(v));
    }
}

int main()
{
    test_type<u8>();
    test_type<u16>();
    test_type<u32>();
    test_type<u64>();
    test_type<u128>();

    test_type<i8>();
    test_type<i16>();
    test_type<i32>();
    test_type<i64>();
    test_type<i128>();

    test_type<f32>();
    test_type<f64>();

    test_float_values();

    return boost::report_errors();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Verifies that the policy-typed aliases integrate with the trait machinery,
// numeric_limits, hashing, streaming, the policy family free functions,
// and the generic policy-parameterized functions

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <functional>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <cstdint>

#endif

using namespace boost::safe_numbers;

void test_traits()
{
    static_assert(detail::is_unsigned_library_type_v<sat_u8>);
    static_assert(detail::is_unsigned_library_type_v<strict_u128>);
    static_assert(detail::is_signed_library_type_v<sat_i32>);
    static_assert(detail::is_signed_library_type_v<strict_i8>);
    static_assert(detail::is_float_library_type_v<sat_f32>);
    static_assert(detail::is_float_library_type_v<sat_f64>);

    static_assert(detail::is_library_type_v<sat_u64>);
    static_assert(detail::is_library_type_v<sat_f64>);
    static_assert(detail::is_integral_library_type_v<sat_i128>);
    static_assert(!detail::is_integral_library_type_v<sat_f32>);

    static_assert(std::is_same_v<detail::underlying_type_t<sat_u32>, std::uint32_t>);
    static_assert(std::is_same_v<detail::underlying_type_t<strict_i64>, std::int64_t>);
    static_assert(std::is_same_v<detail::underlying_type_t<sat_f64>, double>);

    static_assert(std::is_same_v<sat_u8::basis_type, std::uint8_t>);

    // The type-level policy is queryable
    static_assert(u8::error_policy == overflow_policy::throw_exception);
    static_assert(sat_u8::error_policy == overflow_policy::saturate);
    static_assert(strict_u8::error_policy == overflow_policy::strict);
    static_assert(sat_f32::error_policy == overflow_policy::saturate);

    // Layout is unchanged by the policy parameter
    static_assert(sizeof(sat_u8) == sizeof(std::uint8_t));
    static_assert(sizeof(sat_i64) == sizeof(std::int64_t));
    static_assert(std::is_trivially_copyable_v<sat_u64>);
    static_assert(std::is_trivially_copyable_v<sat_f32>);
    static_assert(std::has_unique_object_representations_v<sat_u32>);

    BOOST_TEST(true);
}

void test_numeric_limits()
{
    static_assert(std::numeric_limits<sat_u8>::is_specialized);
    static_assert(std::numeric_limits<strict_i8>::is_specialized);
    static_assert(std::numeric_limits<sat_f32>::is_specialized);

    BOOST_TEST_EQ(static_cast<std::uint8_t>(std::numeric_limits<sat_u8>::max()),
                  static_cast<std::uint8_t>(std::numeric_limits<u8>::max()));
    BOOST_TEST_EQ(static_cast<std::int8_t>(std::numeric_limits<sat_i8>::min()),
                  static_cast<std::int8_t>(std::numeric_limits<i8>::min()));

    static_assert(std::numeric_limits<sat_u8>::digits == std::numeric_limits<u8>::digits);
    static_assert(std::numeric_limits<sat_i64>::is_signed);
    static_assert(std::numeric_limits<sat_f64>::has_infinity);
    static_assert(!std::numeric_limits<sat_u128>::is_signed);
}

void test_hash_and_stream()
{
    const auto h1 {std::hash<sat_u8>{}(sat_u8{42U})};
    const auto h2 {std::hash<u8>{}(u8{42U})};
    BOOST_TEST_EQ(h1, h2);

    std::ostringstream os;
    os << sat_u16{1000U} << " " << strict_i16{std::int16_t{-5}} << " " << sat_f32{1.5F};
    BOOST_TEST_EQ(os.str(), "1000 -5 1.5");

    std::istringstream is {"77"};
    sat_u8 parsed {};
    is >> parsed;
    BOOST_TEST_EQ(static_cast<std::uint8_t>(parsed), 77U);
}

// The named families accept policy-typed operands and preserve the policy
void test_free_functions_on_policy_types()
{
    const sat_u8 sa {200U};
    const sat_u8 sb {100U};

    const auto sat_res {saturating_add(sa, sb)};
    static_assert(std::is_same_v<decltype(sat_res), const sat_u8>);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(sat_res), 255U);

    const auto ovf {overflowing_add(sa, sb)};
    static_assert(std::is_same_v<decltype(ovf), const std::pair<sat_u8, bool>>);
    BOOST_TEST(ovf.second);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(ovf.first), static_cast<std::uint8_t>(44U));

    const auto chk {checked_add(sa, sb)};
    static_assert(std::is_same_v<decltype(chk), const std::optional<sat_u8>>);
    BOOST_TEST(!chk.has_value());

    const strict_u8 ta {10U};
    const strict_u8 tb {20U};
    const auto chk2 {checked_add(ta, tb)};
    static_assert(std::is_same_v<decltype(chk2), const std::optional<strict_u8>>);
    BOOST_TEST(chk2.has_value());

    // Widening preserves the operand policy in the wider result
    const auto wide {widening_add(sa, sb)};
    static_assert(std::is_same_v<decltype(wide),
                                 const detail::unsigned_integer_basis<std::uint16_t, overflow_policy::saturate>>);
    BOOST_TEST_EQ(static_cast<std::uint16_t>(wide), 300U);

    const auto fovf {overflowing_add(sat_f32{1.0F}, sat_f32{2.0F})};
    static_assert(std::is_same_v<decltype(fovf), const std::pair<sat_f32, bool>>);
    BOOST_TEST(!fovf.second);
}

// A generic call requesting throw_exception must throw even when the operand
// type itself carries a non-throwing policy
void test_generic_functions_on_policy_types()
{
    const sat_u8 sa {255U};
    const sat_u8 sb {1U};

    bool caught {false};
    try
    {
        const auto res {add<overflow_policy::throw_exception>(sa, sb)};
        static_cast<void>(res);
    }
    catch (const std::overflow_error&)
    {
        caught = true;
    }
    BOOST_TEST(caught);

    const auto sat_res {add<overflow_policy::saturate>(sa, sb)};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(sat_res), 255U);

    const auto tup {add<overflow_policy::overflow_tuple>(sa, sb)};
    BOOST_TEST(tup.second);

    // Same check for the throwing default types, which keep throwing
    caught = false;
    try
    {
        const auto res {add<overflow_policy::throw_exception>(u8{255U}, u8{1U})};
        static_cast<void>(res);
    }
    catch (const std::overflow_error&)
    {
        caught = true;
    }
    BOOST_TEST(caught);
}

// Strict types behave like the default on the success path
void test_strict_success_paths()
{
    const strict_u8 a {10U};
    const strict_u8 b {20U};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(a + b), 30U);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(b - a), 10U);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(a * b), 200U);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(b / a), 2U);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(b % a), 0U);

    const strict_i8 c {std::int8_t{-5}};
    BOOST_TEST_EQ(static_cast<std::int8_t>(-c), std::int8_t{5});

    static_assert(noexcept(a + b));
    static_assert(noexcept(b / a));
    static_assert(strict_u8::error_policy == overflow_policy::strict);
}

// Comparison operators work within one policy
void test_comparisons()
{
    BOOST_TEST(sat_u8{1U} < sat_u8{2U});
    BOOST_TEST(sat_u8{2U} == sat_u8{2U});
    BOOST_TEST(strict_i8{std::int8_t{-1}} < strict_i8{std::int8_t{1}});
    static_assert(noexcept(sat_u8{1U} == sat_u8{2U}));
}

int main()
{
    test_traits();
    test_numeric_limits();
    test_hash_and_stream();
    test_free_functions_on_policy_types();
    test_generic_functions_on_policy_types();
    test_strict_success_paths();
    test_comparisons();

    return boost::report_errors();
}

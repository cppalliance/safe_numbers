// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wsign-compare"
#  pragma clang diagnostic ignored "-Woverflow"

#  if (__clang_major__ >= 10 && !defined(__APPLE__)) || __clang_major__ >= 13
#    pragma clang diagnostic ignored "-Wdeprecated-copy"
#  endif

#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wundef"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Woverflow"

#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4389)
#  pragma warning(disable : 4127)
#  pragma warning(disable : 4305)
#  pragma warning(disable : 4309)
#endif

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_COMPARE
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION

#include <boost/random/uniform_int_distribution.hpp>

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

// Ignore [[nodiscard]] on the test that we know are going to throw
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-result"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(_MSC_VER)
#  pragma warning (push)
#  pragma warning (disable: 4834)
#endif

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <random>
#include <limits>
#include <type_traits>

#endif

using namespace boost::safe_numbers;

inline std::mt19937_64 rng{42};
inline constexpr std::size_t N {1024};

// =============================================
// Bitwise NOT (~)
// =============================================

template <typename T>
void test_bitwise_not()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const T wrapped {raw};
        const auto result {~wrapped};
        const T expected {static_cast<basis_type>(~raw)};

        BOOST_TEST(expected == result);
    }

    // Edge cases
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} == ~T{0});
    BOOST_TEST(T{0} == ~T{std::numeric_limits<basis_type>::max()});
}

// =============================================
// Bitwise AND (&)
// =============================================

template <typename T>
void test_bitwise_and()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_a {dist(rng)};
        const auto raw_b {dist(rng)};
        const T a {raw_a};
        const T b {raw_b};
        const auto result {a & b};
        const T expected {static_cast<basis_type>(raw_a & raw_b)};

        BOOST_TEST(expected == result);
    }

    // Edge cases
    BOOST_TEST(T{0} == (T{0} & T{std::numeric_limits<basis_type>::max()}));
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} == (T{std::numeric_limits<basis_type>::max()} & T{std::numeric_limits<basis_type>::max()}));

    // x & x == x
    const T val {42};
    BOOST_TEST(val == (val & val));
}

// =============================================
// Bitwise OR (|)
// =============================================

template <typename T>
void test_bitwise_or()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_a {dist(rng)};
        const auto raw_b {dist(rng)};
        const T a {raw_a};
        const T b {raw_b};
        const auto result {a | b};
        const T expected {static_cast<basis_type>(raw_a | raw_b)};

        BOOST_TEST(expected == result);
    }

    // Edge cases
    BOOST_TEST(T{std::numeric_limits<basis_type>::max()} == (T{0} | T{std::numeric_limits<basis_type>::max()}));
    BOOST_TEST(T{0} == (T{0} | T{0}));

    // x | 0 == x
    const T val {42};
    BOOST_TEST(val == (val | T{0}));
}

// =============================================
// Bitwise XOR (^)
// =============================================

template <typename T>
void test_bitwise_xor()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_a {dist(rng)};
        const auto raw_b {dist(rng)};
        const T a {raw_a};
        const T b {raw_b};
        const auto result {a ^ b};
        const T expected {static_cast<basis_type>(raw_a ^ raw_b)};

        BOOST_TEST(expected == result);
    }

    // Edge cases: x ^ x == 0
    const T val {42};
    BOOST_TEST(T{0} == (val ^ val));

    // x ^ 0 == x
    BOOST_TEST(val == (val ^ T{0}));
}

// =============================================
// Left Shift (<<) - Success
// =============================================

template <typename T>
void test_left_shift_success()
{
    using basis_type = detail::underlying_type_t<T>;
    constexpr auto digits {std::numeric_limits<basis_type>::digits};

    // Shifting zero by any amount should be fine
    BOOST_TEST(T{0} == (T{0} << T{0}));

    // Shift 1 left by amounts that fit
    for (int i {0}; i < digits - 1; ++i)
    {
        const auto shift {static_cast<basis_type>(i)};
        const auto expected {static_cast<basis_type>(static_cast<basis_type>(1) << shift)};
        const auto result {T{1} << T{shift}};

        BOOST_TEST(T{expected} == result);
    }

    // Shift small values by small amounts
    const T two {2};
    BOOST_TEST(T{4} == (two << T{1}));
    BOOST_TEST(T{8} == (two << T{2}));
}

// =============================================
// Left Shift (<<) - Failure (throws)
// =============================================

template <typename T>
void test_left_shift_failure()
{
    using basis_type = detail::underlying_type_t<T>;
    constexpr auto digits {std::numeric_limits<basis_type>::digits};

    // Shifting by the full type width should throw
    BOOST_TEST_THROWS(T{1} << T{static_cast<basis_type>(digits)}, std::overflow_error);

    // Shifting a large value left should throw
    BOOST_TEST_THROWS(T{std::numeric_limits<basis_type>::max()} << T{1}, std::overflow_error);

    // Shifting the high bit left by 1 should throw
    const auto high_bit {static_cast<basis_type>(static_cast<basis_type>(1) << (digits - 1))};
    BOOST_TEST_THROWS(T{high_bit} << T{1}, std::overflow_error);
}

// =============================================
// Right Shift (>>) - Success
// =============================================

template <typename T>
void test_right_shift_success()
{
    using basis_type = detail::underlying_type_t<T>;
    constexpr auto digits {std::numeric_limits<basis_type>::digits};

    // Shifting zero by any amount should be fine
    BOOST_TEST(T{0} == (T{0} >> T{0}));

    // Shift the high bit right by increasing amounts
    const auto high_bit {static_cast<basis_type>(static_cast<basis_type>(1) << (digits - 1))};
    for (int i {0}; i < digits; ++i)
    {
        const auto shift {static_cast<basis_type>(i)};
        const auto expected {static_cast<basis_type>(high_bit >> shift)};
        const auto result {T{high_bit} >> T{shift}};

        BOOST_TEST(T{expected} == result);
    }

    // Shift a value by 0 returns the same value
    const T val {42};
    BOOST_TEST(val == (val >> T{0}));
}

// =============================================
// Right Shift (>>) - Failure (throws)
// =============================================

template <typename T>
void test_right_shift_failure()
{
    using basis_type = detail::underlying_type_t<T>;
    constexpr auto digits {std::numeric_limits<basis_type>::digits};

    // Shifting by the full type width should throw
    BOOST_TEST_THROWS(T{1} >> T{static_cast<basis_type>(digits)}, std::overflow_error);

    // Shifting by more than the type width should throw
    if constexpr (digits < std::numeric_limits<basis_type>::max())
    {
        BOOST_TEST_THROWS(T{1} >> T{static_cast<basis_type>(digits + 1)}, std::overflow_error);
    }
}

// =============================================
// u128 specific tests (boost::random supports uint128_t)
// =============================================

void test_bitwise_not_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const u128 wrapped {raw};
        const auto result {~wrapped};
        const u128 expected {~raw};

        BOOST_TEST(expected == result);
    }

    BOOST_TEST(u128{std::numeric_limits<basis_type>::max()} == ~u128{0});
    BOOST_TEST(u128{0} == ~u128{std::numeric_limits<basis_type>::max()});
}

void test_bitwise_and_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_a {dist(rng)};
        const auto raw_b {dist(rng)};
        const u128 a {raw_a};
        const u128 b {raw_b};
        const auto result {a & b};
        const u128 expected {raw_a & raw_b};

        BOOST_TEST(expected == result);
    }

    BOOST_TEST(u128{0} == (u128{0} & u128{std::numeric_limits<basis_type>::max()}));

    const u128 val {42};
    BOOST_TEST(val == (val & val));
}

void test_bitwise_or_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_a {dist(rng)};
        const auto raw_b {dist(rng)};
        const u128 a {raw_a};
        const u128 b {raw_b};
        const auto result {a | b};
        const u128 expected {raw_a | raw_b};

        BOOST_TEST(expected == result);
    }

    BOOST_TEST(u128{std::numeric_limits<basis_type>::max()} == (u128{0} | u128{std::numeric_limits<basis_type>::max()}));

    const u128 val {42};
    BOOST_TEST(val == (val | u128{0}));
}

void test_bitwise_xor_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_a {dist(rng)};
        const auto raw_b {dist(rng)};
        const u128 a {raw_a};
        const u128 b {raw_b};
        const auto result {a ^ b};
        const u128 expected {raw_a ^ raw_b};

        BOOST_TEST(expected == result);
    }

    const u128 val {42};
    BOOST_TEST(u128{0} == (val ^ val));
    BOOST_TEST(val == (val ^ u128{0}));
}

void test_left_shift_success_u128()
{
    BOOST_TEST(u128{0} == (u128{0} << u128{0}));

    for (int i {0}; i < 127; ++i)
    {
        using basis_type = detail::underlying_type_t<u128>;
        const auto shift {static_cast<basis_type>(i)};
        const auto expected {static_cast<basis_type>(static_cast<basis_type>(1) << shift)};
        const auto result {u128{1} << u128{shift}};

        BOOST_TEST(u128{expected} == result);
    }

    const u128 two {2};
    BOOST_TEST(u128{4} == (two << u128{1}));
    BOOST_TEST(u128{8} == (two << u128{2}));
}

void test_left_shift_failure_u128()
{
    using basis_type = detail::underlying_type_t<u128>;

    BOOST_TEST_THROWS(u128{1} << u128{128}, std::overflow_error);
    BOOST_TEST_THROWS(u128{std::numeric_limits<basis_type>::max()} << u128{1}, std::overflow_error);
}

void test_right_shift_success_u128()
{
    using basis_type = detail::underlying_type_t<u128>;

    BOOST_TEST(u128{0} == (u128{0} >> u128{0}));

    const auto high_bit {static_cast<basis_type>(static_cast<basis_type>(1) << 127)};
    for (int i {0}; i < 128; ++i)
    {
        const auto shift {static_cast<basis_type>(i)};
        const auto expected {static_cast<basis_type>(high_bit >> shift)};
        const auto result {u128{high_bit} >> u128{shift}};

        BOOST_TEST(u128{expected} == result);
    }

    const u128 val {42};
    BOOST_TEST(val == (val >> u128{0}));
}

void test_right_shift_failure_u128()
{
    BOOST_TEST_THROWS(u128{1} >> u128{128}, std::overflow_error);
    BOOST_TEST_THROWS(u128{1} >> u128{200}, std::overflow_error);
}

int main()
{
    // Bitwise NOT
    test_bitwise_not<u8>();
    test_bitwise_not<u16>();
    test_bitwise_not<u32>();
    test_bitwise_not<u64>();
    test_bitwise_not_u128();

    // Bitwise AND
    test_bitwise_and<u8>();
    test_bitwise_and<u16>();
    test_bitwise_and<u32>();
    test_bitwise_and<u64>();
    test_bitwise_and_u128();

    // Bitwise OR
    test_bitwise_or<u8>();
    test_bitwise_or<u16>();
    test_bitwise_or<u32>();
    test_bitwise_or<u64>();
    test_bitwise_or_u128();

    // Bitwise XOR
    test_bitwise_xor<u8>();
    test_bitwise_xor<u16>();
    test_bitwise_xor<u32>();
    test_bitwise_xor<u64>();
    test_bitwise_xor_u128();

    // Left shift - success
    test_left_shift_success<u8>();
    test_left_shift_success<u16>();
    test_left_shift_success<u32>();
    test_left_shift_success<u64>();
    test_left_shift_success_u128();

    // Left shift - failure
    test_left_shift_failure<u8>();
    test_left_shift_failure<u16>();
    test_left_shift_failure<u32>();
    test_left_shift_failure<u64>();
    test_left_shift_failure_u128();

    // Right shift - success
    test_right_shift_success<u8>();
    test_right_shift_success<u16>();
    test_right_shift_success<u32>();
    test_right_shift_success<u64>();
    test_right_shift_success_u128();

    // Right shift - failure
    test_right_shift_failure<u8>();
    test_right_shift_failure<u16>();
    test_right_shift_failure<u32>();
    test_right_shift_failure<u64>();
    test_right_shift_failure_u128();

    return boost::report_errors();
}

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

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <boost/safe_numbers/bit.hpp>
#include <random>
#include <limits>
#include <type_traits>
#include <bit>

#endif

using namespace boost::safe_numbers;

inline std::mt19937_64 rng{42};
inline constexpr std::size_t N {1024};

template <typename T>
void test_has_single_bit()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::has_single_bit(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::has_single_bit(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }

    // Edge cases
    BOOST_TEST_EQ(std::has_single_bit(static_cast<basis_type>(0)), boost::safe_numbers::has_single_bit(T{0}));
    BOOST_TEST_EQ(std::has_single_bit(static_cast<basis_type>(1)), boost::safe_numbers::has_single_bit(T{1}));
    BOOST_TEST_EQ(std::has_single_bit(static_cast<basis_type>(2)), boost::safe_numbers::has_single_bit(T{2}));
}

template <typename T>
void test_bit_ceil()
{
    using basis_type = detail::underlying_type_t<T>;

    // bit_ceil has UB if the result isn't representable, so use small values
    boost::random::uniform_int_distribution<basis_type> dist {1, static_cast<basis_type>(std::numeric_limits<basis_type>::max() >> 1)};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::bit_ceil(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::bit_ceil(wrapped)};

        BOOST_TEST(T{expected} == result);
    }

    // Edge cases
    BOOST_TEST(T{std::bit_ceil(static_cast<basis_type>(1))} == boost::safe_numbers::bit_ceil(T{1}));
    BOOST_TEST(T{std::bit_ceil(static_cast<basis_type>(2))} == boost::safe_numbers::bit_ceil(T{2}));
    BOOST_TEST(T{std::bit_ceil(static_cast<basis_type>(3))} == boost::safe_numbers::bit_ceil(T{3}));
}

template <typename T>
void test_bit_floor()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::bit_floor(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::bit_floor(wrapped)};

        BOOST_TEST(T{expected} == result);
    }

    // Edge cases
    BOOST_TEST(T{std::bit_floor(static_cast<basis_type>(0))} == boost::safe_numbers::bit_floor(T{0}));
    BOOST_TEST(T{std::bit_floor(static_cast<basis_type>(1))} == boost::safe_numbers::bit_floor(T{1}));
    BOOST_TEST(T{std::bit_floor(static_cast<basis_type>(7))} == boost::safe_numbers::bit_floor(T{7}));
}

template <typename T>
void test_bit_width()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::bit_width(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::bit_width(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }

    // Edge cases
    BOOST_TEST_EQ(std::bit_width(static_cast<basis_type>(0)), boost::safe_numbers::bit_width(T{0}));
    BOOST_TEST_EQ(std::bit_width(static_cast<basis_type>(1)), boost::safe_numbers::bit_width(T{1}));
    BOOST_TEST_EQ(std::bit_width(std::numeric_limits<basis_type>::max()), boost::safe_numbers::bit_width(T{std::numeric_limits<basis_type>::max()}));
}

template <typename T>
void test_rotl()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto s {static_cast<int>(i % (sizeof(basis_type) * 8U))};
        const auto expected {std::rotl(raw, s)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::rotl(wrapped, s)};

        BOOST_TEST(T{expected} == result);
    }
}

template <typename T>
void test_rotr()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto s {static_cast<int>(i % (sizeof(basis_type) * 8U))};
        const auto expected {std::rotr(raw, s)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::rotr(wrapped, s)};

        BOOST_TEST(T{expected} == result);
    }
}

template <typename T>
void test_countl_zero()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::countl_zero(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::countl_zero(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }

    // Edge cases
    BOOST_TEST_EQ(std::countl_zero(static_cast<basis_type>(0)), boost::safe_numbers::countl_zero(T{0}));
    BOOST_TEST_EQ(std::countl_zero(std::numeric_limits<basis_type>::max()), boost::safe_numbers::countl_zero(T{std::numeric_limits<basis_type>::max()}));
}

template <typename T>
void test_countl_one()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::countl_one(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::countl_one(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }

    // Edge cases
    BOOST_TEST_EQ(std::countl_one(static_cast<basis_type>(0)), boost::safe_numbers::countl_one(T{0}));
    BOOST_TEST_EQ(std::countl_one(std::numeric_limits<basis_type>::max()), boost::safe_numbers::countl_one(T{std::numeric_limits<basis_type>::max()}));
}

template <typename T>
void test_countr_zero()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::countr_zero(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::countr_zero(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }

    // Edge cases
    BOOST_TEST_EQ(std::countr_zero(static_cast<basis_type>(0)), boost::safe_numbers::countr_zero(T{0}));
    BOOST_TEST_EQ(std::countr_zero(std::numeric_limits<basis_type>::max()), boost::safe_numbers::countr_zero(T{std::numeric_limits<basis_type>::max()}));
}

template <typename T>
void test_countr_one()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::countr_one(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::countr_one(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }

    // Edge cases
    BOOST_TEST_EQ(std::countr_one(static_cast<basis_type>(0)), boost::safe_numbers::countr_one(T{0}));
    BOOST_TEST_EQ(std::countr_one(std::numeric_limits<basis_type>::max()), boost::safe_numbers::countr_one(T{std::numeric_limits<basis_type>::max()}));
}

template <typename T>
void test_popcount()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::popcount(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::popcount(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }

    // Edge cases
    BOOST_TEST_EQ(std::popcount(static_cast<basis_type>(0)), boost::safe_numbers::popcount(T{0}));
    BOOST_TEST_EQ(std::popcount(std::numeric_limits<basis_type>::max()), boost::safe_numbers::popcount(T{std::numeric_limits<basis_type>::max()}));
}

template <typename T>
void test_byteswap()
{
    using basis_type = detail::underlying_type_t<T>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::core::byteswap(raw)};
        const T wrapped {raw};
        const auto result {boost::safe_numbers::byteswap(wrapped)};

        BOOST_TEST(T{expected} == result);
    }

    // Edge cases
    BOOST_TEST(T{boost::core::byteswap(static_cast<basis_type>(0))} == boost::safe_numbers::byteswap(T{0}));
    BOOST_TEST(T{boost::core::byteswap(std::numeric_limits<basis_type>::max())} == boost::safe_numbers::byteswap(T{std::numeric_limits<basis_type>::max()}));
}

// u128 tests use boost::int128 functions as reference since std:: doesn't support 128-bit types

void test_has_single_bit_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::has_single_bit(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::has_single_bit(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }
}

void test_bit_ceil_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {1, static_cast<basis_type>(std::numeric_limits<basis_type>::max() >> 1)};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::bit_ceil(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::bit_ceil(wrapped)};

        BOOST_TEST(u128{expected} == result);
    }
}

void test_bit_floor_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::bit_floor(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::bit_floor(wrapped)};

        BOOST_TEST(u128{expected} == result);
    }
}

void test_bit_width_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::bit_width(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::bit_width(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }
}

void test_rotl_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto s {static_cast<int>(i % 128U)};
        const auto expected {boost::int128::rotl(raw, s)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::rotl(wrapped, s)};

        BOOST_TEST(u128{expected} == result);
    }
}

void test_rotr_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto s {static_cast<int>(i % 128U)};
        const auto expected {boost::int128::rotr(raw, s)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::rotr(wrapped, s)};

        BOOST_TEST(u128{expected} == result);
    }
}

void test_countl_zero_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::countl_zero(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::countl_zero(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }
}

void test_countl_one_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::countl_one(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::countl_one(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }
}

void test_countr_zero_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::countr_zero(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::countr_zero(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }
}

void test_countr_one_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::countr_one(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::countr_one(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }
}

void test_popcount_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::popcount(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::popcount(wrapped)};

        BOOST_TEST_EQ(expected, result);
    }
}

void test_byteswap_u128()
{
    using basis_type = detail::underlying_type_t<u128>;
    boost::random::uniform_int_distribution<basis_type> dist {0, std::numeric_limits<basis_type>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {boost::int128::byteswap(raw)};
        const u128 wrapped {raw};
        const auto result {boost::safe_numbers::byteswap(wrapped)};

        BOOST_TEST(u128{expected} == result);
    }
}

// -----------------------------------------------
// bounded_uint types covering full underlying range
// -----------------------------------------------

using bounded_u8_full  = bounded_uint<0u, 255u>;
using bounded_u16_full = bounded_uint<0u, 65535u>;
using bounded_u32_full = bounded_uint<0u, 4294967295u>;
using bounded_u64_full = bounded_uint<0ULL, UINT64_MAX>;

// -----------------------------------------------
// Narrower bounded_uint tests
// -----------------------------------------------

template <typename BoundedT, typename UnderlyingT>
void test_bounded_has_single_bit(UnderlyingT lo, UnderlyingT hi)
{
    boost::random::uniform_int_distribution<UnderlyingT> dist {lo, hi};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::has_single_bit(raw)};
        const BoundedT wrapped {raw};
        BOOST_TEST_EQ(expected, boost::safe_numbers::has_single_bit(wrapped));
    }
}

template <typename BoundedT, typename UnderlyingT>
void test_bounded_bit_width(UnderlyingT lo, UnderlyingT hi)
{
    boost::random::uniform_int_distribution<UnderlyingT> dist {lo, hi};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::bit_width(raw)};
        const BoundedT wrapped {raw};
        BOOST_TEST_EQ(expected, boost::safe_numbers::bit_width(wrapped));
    }
}

template <typename BoundedT, typename UnderlyingT>
void test_bounded_popcount(UnderlyingT lo, UnderlyingT hi)
{
    boost::random::uniform_int_distribution<UnderlyingT> dist {lo, hi};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::popcount(raw)};
        const BoundedT wrapped {raw};
        BOOST_TEST_EQ(expected, boost::safe_numbers::popcount(wrapped));
    }
}

template <typename BoundedT, typename UnderlyingT>
void test_bounded_countl_zero(UnderlyingT lo, UnderlyingT hi)
{
    boost::random::uniform_int_distribution<UnderlyingT> dist {lo, hi};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::countl_zero(raw)};
        const BoundedT wrapped {raw};
        BOOST_TEST_EQ(expected, boost::safe_numbers::countl_zero(wrapped));
    }
}

template <typename BoundedT, typename UnderlyingT>
void test_bounded_countr_zero(UnderlyingT lo, UnderlyingT hi)
{
    boost::random::uniform_int_distribution<UnderlyingT> dist {lo, hi};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto raw {dist(rng)};
        const auto expected {std::countr_zero(raw)};
        const BoundedT wrapped {raw};
        BOOST_TEST_EQ(expected, boost::safe_numbers::countr_zero(wrapped));
    }
}

void test_narrow_bounded_bit_ops()
{
    using narrow_u8  = bounded_uint<10u, 200u>;
    using narrow_u16 = bounded_uint<256u, 40000u>;
    using narrow_u32 = bounded_uint<1000u, 100000u>;

    // has_single_bit
    test_bounded_has_single_bit<narrow_u8>(std::uint8_t{10}, std::uint8_t{200});
    test_bounded_has_single_bit<narrow_u16>(std::uint16_t{256}, std::uint16_t{40000});
    test_bounded_has_single_bit<narrow_u32>(std::uint32_t{1000}, std::uint32_t{100000});

    // bit_width
    test_bounded_bit_width<narrow_u8>(std::uint8_t{10}, std::uint8_t{200});
    test_bounded_bit_width<narrow_u16>(std::uint16_t{256}, std::uint16_t{40000});
    test_bounded_bit_width<narrow_u32>(std::uint32_t{1000}, std::uint32_t{100000});

    // popcount
    test_bounded_popcount<narrow_u8>(std::uint8_t{10}, std::uint8_t{200});
    test_bounded_popcount<narrow_u16>(std::uint16_t{256}, std::uint16_t{40000});
    test_bounded_popcount<narrow_u32>(std::uint32_t{1000}, std::uint32_t{100000});

    // countl_zero
    test_bounded_countl_zero<narrow_u8>(std::uint8_t{10}, std::uint8_t{200});
    test_bounded_countl_zero<narrow_u16>(std::uint16_t{256}, std::uint16_t{40000});
    test_bounded_countl_zero<narrow_u32>(std::uint32_t{1000}, std::uint32_t{100000});

    // countr_zero
    test_bounded_countr_zero<narrow_u8>(std::uint8_t{10}, std::uint8_t{200});
    test_bounded_countr_zero<narrow_u16>(std::uint16_t{256}, std::uint16_t{40000});
    test_bounded_countr_zero<narrow_u32>(std::uint32_t{1000}, std::uint32_t{100000});
}

int main()
{
    test_has_single_bit<u8>();
    test_has_single_bit<u16>();
    test_has_single_bit<u32>();
    test_has_single_bit<u64>();
    test_has_single_bit_u128();

    test_bit_ceil<u8>();
    test_bit_ceil<u16>();
    test_bit_ceil<u32>();
    test_bit_ceil<u64>();
    test_bit_ceil_u128();

    test_bit_floor<u8>();
    test_bit_floor<u16>();
    test_bit_floor<u32>();
    test_bit_floor<u64>();
    test_bit_floor_u128();

    test_bit_width<u8>();
    test_bit_width<u16>();
    test_bit_width<u32>();
    test_bit_width<u64>();
    test_bit_width_u128();

    test_rotl<u8>();
    test_rotl<u16>();
    test_rotl<u32>();
    test_rotl<u64>();
    test_rotl_u128();

    test_rotr<u8>();
    test_rotr<u16>();
    test_rotr<u32>();
    test_rotr<u64>();
    test_rotr_u128();

    test_countl_zero<u8>();
    test_countl_zero<u16>();
    test_countl_zero<u32>();
    test_countl_zero<u64>();
    test_countl_zero_u128();

    test_countl_one<u8>();
    test_countl_one<u16>();
    test_countl_one<u32>();
    test_countl_one<u64>();
    test_countl_one_u128();

    test_countr_zero<u8>();
    test_countr_zero<u16>();
    test_countr_zero<u32>();
    test_countr_zero<u64>();
    test_countr_zero_u128();

    test_countr_one<u8>();
    test_countr_one<u16>();
    test_countr_one<u32>();
    test_countr_one<u64>();
    test_countr_one_u128();

    test_popcount<u8>();
    test_popcount<u16>();
    test_popcount<u32>();
    test_popcount<u64>();
    test_popcount_u128();

    test_byteswap<u8>();
    test_byteswap<u16>();
    test_byteswap<u32>();
    test_byteswap<u64>();

    test_byteswap_u128();

    // Full-range bounded_uint tests (reuse existing templates)
    test_has_single_bit<bounded_u8_full>();
    test_has_single_bit<bounded_u16_full>();
    test_has_single_bit<bounded_u32_full>();
    test_has_single_bit<bounded_u64_full>();

    test_bit_ceil<bounded_u8_full>();
    test_bit_ceil<bounded_u16_full>();
    test_bit_ceil<bounded_u32_full>();
    test_bit_ceil<bounded_u64_full>();

    test_bit_floor<bounded_u8_full>();
    test_bit_floor<bounded_u16_full>();
    test_bit_floor<bounded_u32_full>();
    test_bit_floor<bounded_u64_full>();

    test_bit_width<bounded_u8_full>();
    test_bit_width<bounded_u16_full>();
    test_bit_width<bounded_u32_full>();
    test_bit_width<bounded_u64_full>();

    test_countl_zero<bounded_u8_full>();
    test_countl_zero<bounded_u16_full>();
    test_countl_zero<bounded_u32_full>();
    test_countl_zero<bounded_u64_full>();

    test_countl_one<bounded_u8_full>();
    test_countl_one<bounded_u16_full>();
    test_countl_one<bounded_u32_full>();
    test_countl_one<bounded_u64_full>();

    test_countr_zero<bounded_u8_full>();
    test_countr_zero<bounded_u16_full>();
    test_countr_zero<bounded_u32_full>();
    test_countr_zero<bounded_u64_full>();

    test_countr_one<bounded_u8_full>();
    test_countr_one<bounded_u16_full>();
    test_countr_one<bounded_u32_full>();
    test_countr_one<bounded_u64_full>();

    test_popcount<bounded_u8_full>();
    test_popcount<bounded_u16_full>();
    test_popcount<bounded_u32_full>();
    test_popcount<bounded_u64_full>();

    // Narrow bounded_uint tests (functions that always return valid results)
    test_narrow_bounded_bit_ops();

    return boost::report_errors();
}

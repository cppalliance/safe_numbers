// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

#include <boost/core/lightweight_test.hpp>
#include <limits>

template <typename T>
void test()
{
    using basis_type = boost::safe_numbers::detail::underlying_type_t<T>;

    BOOST_TEST_EQ(std::numeric_limits<T>::is_specialized, std::numeric_limits<basis_type>::is_specialized);
    BOOST_TEST_EQ(std::numeric_limits<T>::is_signed, std::numeric_limits<basis_type>::is_signed);
    BOOST_TEST_EQ(std::numeric_limits<T>::is_integer, std::numeric_limits<basis_type>::is_integer);
    BOOST_TEST_EQ(std::numeric_limits<T>::is_exact, std::numeric_limits<basis_type>::is_exact);
    BOOST_TEST_EQ(std::numeric_limits<T>::has_infinity, std::numeric_limits<basis_type>::has_infinity);
    BOOST_TEST_EQ(std::numeric_limits<T>::has_quiet_NaN, std::numeric_limits<basis_type>::has_quiet_NaN);
    BOOST_TEST_EQ(std::numeric_limits<T>::has_signaling_NaN, std::numeric_limits<basis_type>::has_signaling_NaN);

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    BOOST_TEST(std::numeric_limits<T>::has_denorm == std::numeric_limits<basis_type>::has_denorm);
    BOOST_TEST(std::numeric_limits<T>::has_denorm_loss == std::numeric_limits<basis_type>::has_denorm_loss);
    #endif

    BOOST_TEST(std::numeric_limits<T>::round_style == std::numeric_limits<basis_type>::round_style);

    BOOST_TEST_EQ(std::numeric_limits<T>::is_iec559, std::numeric_limits<basis_type>::is_iec559);
    BOOST_TEST_EQ(std::numeric_limits<T>::is_bounded, std::numeric_limits<basis_type>::is_bounded);
    BOOST_TEST_EQ(std::numeric_limits<T>::is_modulo, std::numeric_limits<basis_type>::is_modulo);
    BOOST_TEST_EQ(std::numeric_limits<T>::digits, std::numeric_limits<basis_type>::digits);
    BOOST_TEST_EQ(std::numeric_limits<T>::digits10, std::numeric_limits<basis_type>::digits10);
    BOOST_TEST_EQ(std::numeric_limits<T>::max_digits10, std::numeric_limits<basis_type>::max_digits10);
    BOOST_TEST_EQ(std::numeric_limits<T>::radix, std::numeric_limits<basis_type>::radix);
    BOOST_TEST_EQ(std::numeric_limits<T>::min_exponent, std::numeric_limits<basis_type>::min_exponent);
    BOOST_TEST_EQ(std::numeric_limits<T>::min_exponent10, std::numeric_limits<basis_type>::min_exponent10);
    BOOST_TEST_EQ(std::numeric_limits<T>::max_exponent, std::numeric_limits<basis_type>::max_exponent);
    BOOST_TEST_EQ(std::numeric_limits<T>::min_exponent10, std::numeric_limits<basis_type>::min_exponent10);
    BOOST_TEST_EQ(std::numeric_limits<T>::traps, std::numeric_limits<basis_type>::traps);
    BOOST_TEST_EQ(std::numeric_limits<T>::tinyness_before, std::numeric_limits<basis_type>::tinyness_before);

    BOOST_TEST_EQ(std::numeric_limits<T>::min(), T{std::numeric_limits<basis_type>::min()});
    BOOST_TEST_EQ(std::numeric_limits<T>::max(), T{std::numeric_limits<basis_type>::max()});
    BOOST_TEST_EQ(std::numeric_limits<T>::lowest(), T{std::numeric_limits<basis_type>::lowest()});
    BOOST_TEST_EQ(std::numeric_limits<T>::epsilon(), T{std::numeric_limits<basis_type>::epsilon()});
    BOOST_TEST_EQ(std::numeric_limits<T>::round_error(), T{std::numeric_limits<basis_type>::round_error()});
    BOOST_TEST_EQ(std::numeric_limits<T>::infinity(), T{std::numeric_limits<basis_type>::infinity()});
    BOOST_TEST_EQ(std::numeric_limits<T>::denorm_min(), T{std::numeric_limits<basis_type>::denorm_min()});
}

template <typename BoundedT, typename UnderlyingT, UnderlyingT ExpectedMin, UnderlyingT ExpectedMax>
void test_bounded()
{
    using underlying_type = UnderlyingT;

    // Static properties should match the underlying hardware type
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::is_specialized, std::numeric_limits<underlying_type>::is_specialized);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::is_signed, std::numeric_limits<underlying_type>::is_signed);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::is_integer, std::numeric_limits<underlying_type>::is_integer);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::is_exact, std::numeric_limits<underlying_type>::is_exact);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::has_infinity, std::numeric_limits<underlying_type>::has_infinity);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::has_quiet_NaN, std::numeric_limits<underlying_type>::has_quiet_NaN);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::has_signaling_NaN, std::numeric_limits<underlying_type>::has_signaling_NaN);

    #if ((!defined(_MSC_VER) && (__cplusplus <= 202002L)) || (defined(_MSC_VER) && (_MSVC_LANG <= 202002L)))
    BOOST_TEST(std::numeric_limits<BoundedT>::has_denorm == std::numeric_limits<underlying_type>::has_denorm);
    BOOST_TEST(std::numeric_limits<BoundedT>::has_denorm_loss == std::numeric_limits<underlying_type>::has_denorm_loss);
    #endif

    BOOST_TEST(std::numeric_limits<BoundedT>::round_style == std::numeric_limits<underlying_type>::round_style);

    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::is_iec559, std::numeric_limits<underlying_type>::is_iec559);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::is_bounded, std::numeric_limits<underlying_type>::is_bounded);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::is_modulo, std::numeric_limits<underlying_type>::is_modulo);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::digits, std::numeric_limits<underlying_type>::digits);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::digits10, std::numeric_limits<underlying_type>::digits10);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::max_digits10, std::numeric_limits<underlying_type>::max_digits10);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::radix, std::numeric_limits<underlying_type>::radix);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::min_exponent, std::numeric_limits<underlying_type>::min_exponent);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::min_exponent10, std::numeric_limits<underlying_type>::min_exponent10);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::max_exponent, std::numeric_limits<underlying_type>::max_exponent);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::max_exponent10, std::numeric_limits<underlying_type>::max_exponent10);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::traps, std::numeric_limits<underlying_type>::traps);
    BOOST_TEST_EQ(std::numeric_limits<BoundedT>::tinyness_before, std::numeric_limits<underlying_type>::tinyness_before);

    // min() and max() should return the bounds, not the hardware limits
    BOOST_TEST(std::numeric_limits<BoundedT>::min() == BoundedT{ExpectedMin});
    BOOST_TEST(std::numeric_limits<BoundedT>::max() == BoundedT{ExpectedMax});
    BOOST_TEST(std::numeric_limits<BoundedT>::lowest() == std::numeric_limits<BoundedT>::min());
}

int main()
{
    using namespace boost::safe_numbers;

    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();

    // Full-range bounded types
    test_bounded<bounded_uint<0u, 255u>, std::uint8_t, 0, 255>();
    test_bounded<bounded_uint<0u, 65535u>, std::uint16_t, 0, 65535>();
    test_bounded<bounded_uint<0u, 4294967295u>, std::uint32_t, 0, 4294967295u>();
    test_bounded<bounded_uint<0ULL, UINT64_MAX>, std::uint64_t, 0, UINT64_MAX>();

    // Non-zero minimum bounded types
    test_bounded<bounded_uint<10u, 200u>, std::uint8_t, 10, 200>();
    test_bounded<bounded_uint<256u, 40000u>, std::uint16_t, 256, 40000>();
    test_bounded<bounded_uint<1000u, 100000u>, std::uint32_t, 1000, 100000>();
    test_bounded<bounded_uint<4294967296ULL, UINT64_MAX>, std::uint64_t, 4294967296ULL, UINT64_MAX>();

    return boost::report_errors();
}

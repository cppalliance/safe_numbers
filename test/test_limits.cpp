// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
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

int main()
{
    using namespace boost::safe_numbers;

    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();

    return boost::report_errors();
}

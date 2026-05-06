// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

#include <boost/core/lightweight_test.hpp>

#include <limits>
#include <stdexcept>

using namespace boost::safe_numbers;
using namespace boost::safe_numbers::literals;

template <typename T>
void test();

template <>
void test<i8>()
{
    constexpr auto good_value {42_i8};
    BOOST_TEST_EQ(good_value, i8{42});

    // Negative values are formed by applying the wrapper's unary minus to the
    // positive literal: -42_i8 parses as -(42_i8).
    constexpr auto neg_value {-42_i8};
    BOOST_TEST_EQ(neg_value, i8{-42});

    // INT8_MAX itself is representable.
    constexpr auto max_value {127_i8};
    BOOST_TEST_EQ(max_value, i8{127});

    // INT8_MIN+1 is the lowest negative reachable through the integer UDL,
    // because the magnitude (127) fits within INT8_MAX.
    constexpr auto min_plus_one {-127_i8};
    BOOST_TEST_EQ(min_plus_one, i8{-127});

    // INT8_MAX+1 overflows the positive-magnitude check. Note that this also
    // means INT8_MIN is unreachable through the UDL: -128_i8 parses as
    // -(128_i8) and the inner literal throws before the unary minus runs.
    BOOST_TEST_THROWS(128_i8, std::overflow_error);
    BOOST_TEST_THROWS(255_i8, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_i8, std::overflow_error);
}

template <>
void test<i16>()
{
    constexpr auto good_value {42_i16};
    BOOST_TEST_EQ(good_value, i16{42});

    constexpr auto neg_value {-42_i16};
    BOOST_TEST_EQ(neg_value, i16{-42});

    constexpr auto max_value {32767_i16};
    BOOST_TEST_EQ(max_value, i16{32767});

    constexpr auto min_plus_one {-32767_i16};
    BOOST_TEST_EQ(min_plus_one, i16{-32767});

    BOOST_TEST_THROWS(32768_i16, std::overflow_error);
    BOOST_TEST_THROWS(65535_i16, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_i16, std::overflow_error);
}

template <>
void test<i32>()
{
    constexpr auto good_value {42_i32};
    BOOST_TEST_EQ(good_value, i32{42});

    constexpr auto neg_value {-42_i32};
    BOOST_TEST_EQ(neg_value, i32{-42});

    constexpr auto max_value {2147483647_i32};
    BOOST_TEST_EQ(max_value, i32{2147483647});

    constexpr auto min_plus_one {-2147483647_i32};
    BOOST_TEST_EQ(min_plus_one, i32{-2147483647});

    BOOST_TEST_THROWS(2147483648_i32, std::overflow_error);
    BOOST_TEST_THROWS(4294967295_i32, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_i32, std::overflow_error);
}

template <>
void test<i64>()
{
    constexpr auto good_value {42_i64};
    BOOST_TEST_EQ(good_value, i64{42});

    constexpr auto neg_value {-42_i64};
    BOOST_TEST_EQ(neg_value, i64{-42});

    constexpr auto max_value {9223372036854775807_i64};
    BOOST_TEST_EQ(max_value, i64{9223372036854775807LL});

    constexpr auto min_plus_one {-9223372036854775807_i64};
    BOOST_TEST_EQ(min_plus_one, i64{-9223372036854775807LL});

    // INT64_MAX+1 = 2^63 fits in unsigned long long, so the literal token is
    // well-formed; the UDL's range check rejects it as overflow.
    BOOST_TEST_THROWS(9223372036854775808_i64, std::overflow_error);
    BOOST_TEST_THROWS(18446744073709551615_i64, std::overflow_error);
}

template <>
void test<i128>()
{
    // _i128 is the raw-integer UDL form, the same as _u128: it dispatches on
    // an integer-form token and receives the digits as a null-terminated
    // string. Parsing is delegated to from_chars on int128_t.
    constexpr auto good_value {42_i128};
    BOOST_TEST_EQ(good_value, i128{42});

    constexpr auto neg_value {-42_i128};
    BOOST_TEST_EQ(neg_value, i128{-42});

    constexpr auto zero_value {0_i128};
    BOOST_TEST_EQ(zero_value, i128{0});

    // INT128_MAX (2^127 - 1) is representable as a positive literal.
    constexpr auto max_value {170141183460469231731687303715884105727_i128};
    BOOST_TEST_EQ(max_value, i128{(std::numeric_limits<i128::basis_type>::max)()});

    // INT128_MAX+1 (2^127) overflows. As with the smaller signed types, this
    // also means INT128_MIN is unreachable through the UDL.
    BOOST_TEST_THROWS(170141183460469231731687303715884105728_i128, std::overflow_error);
    BOOST_TEST_THROWS(999999999999999999999999999999999999999_i128, std::overflow_error);
    BOOST_TEST_THROWS(1000000000000000000000000000000000000000_i128, std::overflow_error);
}

int main()
{
    test<i8>();
    test<i16>();
    test<i32>();
    test<i64>();
    test<i128>();

    return boost::report_errors();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/safe_numbers/random.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>
#include <limits>

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION
#  error "Should not be defined for the user"
#endif

using namespace boost::safe_numbers;

static std::mt19937_64 rng{42};

template <typename T>
void test()
{
    boost::random::uniform_int_distribution<T> dist{std::numeric_limits<T>::min(), std::numeric_limits<T>::max()};
    const auto val {dist(rng)};
    BOOST_TEST_GE(val, std::numeric_limits<T>::min());
    BOOST_TEST_LE(val, std::numeric_limits<T>::max());

    BOOST_TEST_GE(val, dist.min());
    BOOST_TEST_LE(val, dist.max());

    BOOST_TEST_GE(val, dist.a());
    BOOST_TEST_LE(val, dist.b());

    auto dist2 = dist;

    BOOST_TEST(dist2 == dist);
    BOOST_TEST(!(dist2 != dist));
}

int main()
{
    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();
    test<u128>();

    return boost::report_errors();
}
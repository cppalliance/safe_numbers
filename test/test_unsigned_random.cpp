// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/safe_numbers/random.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>
#include <limits>
#include <sstream>

#ifdef BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION
#  error "Should not be defined for the user"
#endif

using namespace boost::safe_numbers;

static std::mt19937_64 rng{42};

template <typename T>
void test_default_construction()
{
    boost::random::uniform_int_distribution<T> dist;
    BOOST_TEST_EQ(dist.a(), (std::numeric_limits<T>::min)());
    BOOST_TEST_EQ(dist.b(), (std::numeric_limits<T>::max)());
    BOOST_TEST_EQ(dist.min(), (std::numeric_limits<T>::min)());
    BOOST_TEST_EQ(dist.max(), (std::numeric_limits<T>::max)());
}

template <typename T>
void test_range_construction()
{
    auto lo = T{1};
    auto hi = T{10};
    boost::random::uniform_int_distribution<T> dist{lo, hi};
    BOOST_TEST_EQ(dist.a(), lo);
    BOOST_TEST_EQ(dist.b(), hi);
    BOOST_TEST_EQ(dist.min(), lo);
    BOOST_TEST_EQ(dist.max(), hi);
}

template <typename T>
void test_generation()
{
    auto lo = T{1};
    auto hi = T{100};
    boost::random::uniform_int_distribution<T> dist{lo, hi};

    for (int i {}; i < 1000; ++i)
    {
        auto val = dist(rng);
        BOOST_TEST_GE(val, lo);
        BOOST_TEST_LE(val, hi);
    }
}

template <typename T>
void test_generation_with_param()
{
    using dist_t = boost::random::uniform_int_distribution<T>;
    using param_t = typename dist_t::param_type;

    dist_t dist;
    auto lo = T{5};
    auto hi = T{50};
    param_t p{lo, hi};

    for (int i {}; i < 100; ++i)
    {
        auto val = dist(rng, p);
        BOOST_TEST_GE(val, lo);
        BOOST_TEST_LE(val, hi);
    }
}

template <typename T>
void test_param_type()
{
    using dist_t = boost::random::uniform_int_distribution<T>;
    using param_t = typename dist_t::param_type;

    // Default construction
    param_t p_default;
    BOOST_TEST_EQ(p_default.a(), (std::numeric_limits<T>::min)());
    BOOST_TEST_EQ(p_default.b(), (std::numeric_limits<T>::max)());

    // Range construction
    auto lo = T{2};
    auto hi = T{20};
    param_t p{lo, hi};
    BOOST_TEST_EQ(p.a(), lo);
    BOOST_TEST_EQ(p.b(), hi);

    // Equality / inequality
    param_t p2{lo, hi};
    BOOST_TEST(p == p2);
    BOOST_TEST(!(p != p2));
    BOOST_TEST(p != p_default);
    BOOST_TEST(!(p == p_default));
}

template <typename T>
void test_param_construction()
{
    using dist_t = boost::random::uniform_int_distribution<T>;
    using param_t = typename dist_t::param_type;

    auto lo = T{3};
    auto hi = T{30};
    param_t p{lo, hi};

    dist_t dist{p};
    BOOST_TEST_EQ(dist.a(), lo);
    BOOST_TEST_EQ(dist.b(), hi);
}

template <typename T>
void test_param_getter_setter()
{
    using dist_t = boost::random::uniform_int_distribution<T>;
    using param_t = typename dist_t::param_type;

    auto lo = T{1};
    auto hi = T{10};
    dist_t dist{lo, hi};

    // Getter
    auto p = dist.param();
    BOOST_TEST_EQ(p.a(), lo);
    BOOST_TEST_EQ(p.b(), hi);

    // Setter
    auto lo2 = T{5};
    auto hi2 = T{50};
    param_t p2{lo2, hi2};
    dist.param(p2);
    BOOST_TEST_EQ(dist.a(), lo2);
    BOOST_TEST_EQ(dist.b(), hi2);
}

template <typename T>
void test_reset()
{
    boost::random::uniform_int_distribution<T> dist;
    dist.reset();
    auto val = dist(rng);
    BOOST_TEST_GE(val, (std::numeric_limits<T>::min)());
    BOOST_TEST_LE(val, (std::numeric_limits<T>::max)());
}

template <typename T>
void test_equality()
{
    auto lo = T{1};
    auto hi = T{10};
    boost::random::uniform_int_distribution<T> dist1{lo, hi};
    boost::random::uniform_int_distribution<T> dist2{lo, hi};
    boost::random::uniform_int_distribution<T> dist3{lo, T{20}};

    BOOST_TEST(dist1 == dist2);
    BOOST_TEST(!(dist1 != dist2));
    BOOST_TEST(dist1 != dist3);
    BOOST_TEST(!(dist1 == dist3));
}

template <typename T>
void test_dist_stream_roundtrip()
{
    auto lo = T{3};
    auto hi = T{42};
    boost::random::uniform_int_distribution<T> dist{lo, hi};

    std::stringstream ss;
    ss << dist;

    boost::random::uniform_int_distribution<T> dist2;
    ss >> dist2;

    BOOST_TEST(dist == dist2);
    BOOST_TEST_EQ(dist2.a(), lo);
    BOOST_TEST_EQ(dist2.b(), hi);
}

template <typename T>
void test_param_stream_roundtrip()
{
    using dist_t = boost::random::uniform_int_distribution<T>;
    using param_t = typename dist_t::param_type;

    auto lo = T{7};
    auto hi = T{77};
    param_t p{lo, hi};

    std::stringstream ss;
    ss << p;

    param_t p2;
    ss >> p2;

    BOOST_TEST(p == p2);
    BOOST_TEST_EQ(p2.a(), lo);
    BOOST_TEST_EQ(p2.b(), hi);
}

template <typename T>
void test_param_stream_invalid()
{
    using dist_t = boost::random::uniform_int_distribution<T>;
    using param_t = typename dist_t::param_type;

    // Write max < min (invalid)
    std::stringstream ss;
    ss << T{50} << " " << T{5};

    param_t p;
    ss >> p;

    BOOST_TEST(ss.fail());
}

template <typename T>
void test_all()
{
    test_default_construction<T>();
    test_range_construction<T>();
    test_generation<T>();
    test_generation_with_param<T>();
    test_param_type<T>();
    test_param_construction<T>();
    test_param_getter_setter<T>();
    test_reset<T>();
    test_equality<T>();
    test_dist_stream_roundtrip<T>();
    test_param_stream_roundtrip<T>();
    test_param_stream_invalid<T>();
}

int main()
{
    test_all<u8>();
    test_all<u16>();
    test_all<u32>();
    test_all<u64>();
    test_all<u128>();

    // Bounded types
    test_all<bounded_uint<0u, 100u>>();
    test_all<bounded_uint<0u, 1000u>>();

    return boost::report_errors();
}

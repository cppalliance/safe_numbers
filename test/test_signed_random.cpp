// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/safe_numbers/random.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>
#include <limits>

using namespace boost::safe_numbers;

static std::mt19937_64 rng{42};

template <typename T>
void test_default_construction()
{
    boost::random::uniform_int_distribution<T> dist;
    BOOST_TEST(dist.a() == (std::numeric_limits<T>::min)());
    BOOST_TEST(dist.b() == (std::numeric_limits<T>::max)());
    BOOST_TEST(dist.min() == (std::numeric_limits<T>::min)());
    BOOST_TEST(dist.max() == (std::numeric_limits<T>::max)());
}

template <typename T>
void test_range_construction()
{
    auto lo = T{-5};
    auto hi = T{10};
    boost::random::uniform_int_distribution<T> dist{lo, hi};
    BOOST_TEST(dist.a() == lo);
    BOOST_TEST(dist.b() == hi);
    BOOST_TEST(dist.min() == lo);
    BOOST_TEST(dist.max() == hi);
}

template <typename T>
void test_generation()
{
    auto lo = T{-50};
    auto hi = T{50};
    boost::random::uniform_int_distribution<T> dist{lo, hi};

    for (int i {}; i < 1000; ++i)
    {
        auto val = dist(rng);
        BOOST_TEST(val >= lo);
        BOOST_TEST(val <= hi);
    }
}

template <typename T>
void test_generation_with_param()
{
    using dist_t = boost::random::uniform_int_distribution<T>;
    using param_t = typename dist_t::param_type;

    dist_t dist;
    auto lo = T{-25};
    auto hi = T{25};
    param_t p{lo, hi};

    for (int i {}; i < 100; ++i)
    {
        auto val = dist(rng, p);
        BOOST_TEST(val >= lo);
        BOOST_TEST(val <= hi);
    }
}

template <typename T>
void test_param_type()
{
    using dist_t = boost::random::uniform_int_distribution<T>;
    using param_t = typename dist_t::param_type;

    // Default construction
    param_t p_default;
    BOOST_TEST(p_default.a() == (std::numeric_limits<T>::min)());
    BOOST_TEST(p_default.b() == (std::numeric_limits<T>::max)());

    // Range construction
    auto lo = T{-10};
    auto hi = T{20};
    param_t p{lo, hi};
    BOOST_TEST(p.a() == lo);
    BOOST_TEST(p.b() == hi);

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

    auto lo = T{-3};
    auto hi = T{30};
    param_t p{lo, hi};

    dist_t dist{p};
    BOOST_TEST(dist.a() == lo);
    BOOST_TEST(dist.b() == hi);
}

template <typename T>
void test_param_getter_setter()
{
    using dist_t = boost::random::uniform_int_distribution<T>;
    using param_t = typename dist_t::param_type;

    auto lo = T{-10};
    auto hi = T{10};
    dist_t dist{lo, hi};

    // Getter
    auto p = dist.param();
    BOOST_TEST(p.a() == lo);
    BOOST_TEST(p.b() == hi);

    // Setter
    auto lo2 = T{-50};
    auto hi2 = T{50};
    param_t p2{lo2, hi2};
    dist.param(p2);
    BOOST_TEST(dist.a() == lo2);
    BOOST_TEST(dist.b() == hi2);
}

template <typename T>
void test_reset()
{
    boost::random::uniform_int_distribution<T> dist;
    dist.reset();
    auto val = dist(rng);
    BOOST_TEST(val >= (std::numeric_limits<T>::min)());
    BOOST_TEST(val <= (std::numeric_limits<T>::max)());
}

template <typename T>
void test_equality()
{
    auto lo = T{-10};
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
}

int main()
{
    test_all<i8>();
    test_all<i16>();
    test_all<i32>();
    test_all<i64>();
    test_all<i128>();

    return boost::report_errors();
}

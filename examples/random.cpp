// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers.hpp>
#include <boost/safe_numbers/random.hpp>
#include <iostream>
#include <random>

int main()
{
    using namespace boost::safe_numbers;

    std::mt19937_64 rng{42};

    // Generate uniformly distributed unsigned values
    boost::random::uniform_int_distribution<u32> u32_dist{u32{1}, u32{100}};
    std::cout << "u32 in [1, 100]:  " << u32_dist(rng) << std::endl;

    // Generate uniformly distributed signed values
    boost::random::uniform_int_distribution<i32> i32_dist{i32{-50}, i32{50}};
    std::cout << "i32 in [-50, 50]: " << i32_dist(rng) << std::endl;

    // Default range uses the full range of the type
    boost::random::uniform_int_distribution<u64> u64_dist;
    std::cout << "u64 full range:   " << u64_dist(rng) << std::endl;

    // Use param_type to change range without reconstructing
    using dist_t = boost::random::uniform_int_distribution<u16>;
    dist_t dist;
    dist_t::param_type narrow{u16{0}, u16{10}};
    std::cout << "u16 in [0, 10]:   " << dist(rng, narrow) << std::endl;

    // 128-bit types work too
    boost::random::uniform_int_distribution<u128> u128_dist{u128{0U}, u128{1000U}};
    std::cout << "u128 in [0, 1000]: " << u128_dist(rng) << std::endl;

    return 0;
}

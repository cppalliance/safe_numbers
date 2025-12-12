// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <random>
#include <cstring>
#include <limits>
#include <type_traits>

#endif

inline std::mt19937_64 rng{42};
inline constexpr std::size_t N {1024};

template <typename LHSType, typename RHSType>
void test_mixed_add()
{
    std::uniform_int_distribution<std::uint32_t> dist_u32 {std::numeric_limits<std::uint32_t>::min(),
                                                          std::numeric_limits<std::uint32_t>::max()};

    std::uniform_int_distribution<std::uint64_t> dist_u64 {std::numeric_limits<std::uint64_t>::min(),
                                                          std::numeric_limits<std::uint64_t>::max()};

    for (std::size_t i {}; i < N; ++i)
    {
        const auto lhs_val {dist_u32(rng)};
        const auto rhs_val {dist_u64(rng)};

        LHSType lhs {lhs_val};
        RHSType rhs {rhs_val};

        const auto res {lhs + rhs};
        BOOST_TEST(res > LHSType{0});
    }
}

int main()
{
    using namespace boost::safe_numbers;
    test_mixed_add<u32, u64>();

    return boost::report_errors();
}

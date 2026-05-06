// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers/bounded_floats.hpp>
#include <boost/safe_numbers/limits.hpp>
#include <boost/safe_numbers/floats.hpp>

#endif

#include <limits>
#include <type_traits>

using namespace boost::safe_numbers;

// -----------------------------------------------
// Compile-time properties
// -----------------------------------------------

using BFLow = bounded_float<-1.0f, 1.0f>;
using BFWide = bounded_float<-1.0e100, 1.0e100>;

static_assert(std::numeric_limits<BFLow>::is_specialized);
static_assert(!std::numeric_limits<BFLow>::is_iec559);
static_assert(!std::numeric_limits<BFLow>::has_infinity);
static_assert(!std::numeric_limits<BFLow>::has_quiet_NaN);
static_assert(!std::numeric_limits<BFLow>::has_signaling_NaN);
static_assert(!std::numeric_limits<BFLow>::is_integer);
static_assert(!std::numeric_limits<BFLow>::is_exact);
static_assert(std::numeric_limits<BFLow>::is_bounded);
static_assert(std::numeric_limits<BFLow>::is_signed);

static_assert(std::numeric_limits<BFLow>::digits == std::numeric_limits<float>::digits);
static_assert(std::numeric_limits<BFLow>::digits10 == std::numeric_limits<float>::digits10);
static_assert(std::numeric_limits<BFLow>::radix == std::numeric_limits<float>::radix);

static_assert(std::numeric_limits<BFWide>::digits == std::numeric_limits<double>::digits);
static_assert(std::numeric_limits<BFWide>::digits10 == std::numeric_limits<double>::digits10);

// -----------------------------------------------
// min() / max() / lowest()
// -----------------------------------------------

void test_min_max()
{
    constexpr auto min_val {std::numeric_limits<BFLow>::min()};
    constexpr auto max_val {std::numeric_limits<BFLow>::max()};
    constexpr auto lowest_val {std::numeric_limits<BFLow>::lowest()};

    BOOST_TEST_EQ(static_cast<float>(min_val), -1.0f);
    BOOST_TEST_EQ(static_cast<float>(max_val), 1.0f);
    BOOST_TEST_EQ(static_cast<float>(lowest_val), -1.0f);
}

int main()
{
    test_min_max();

    return boost::report_errors();
}

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// The overflowing float family must agree with the throwing operators on
// every input: the bool is true exactly when the operator throws, and when it
// does not throw the value is bit-identical to the operator's result.

#include <boost/safe_numbers/floats.hpp>
#include <boost/core/lightweight_test.hpp>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <bit>
#include <cstdint>
#include <exception>
#include <limits>
#include <type_traits>
#include <vector>

using namespace boost::safe_numbers;

template <typename BasisType>
using bits_type = std::conditional_t<sizeof(BasisType) == 4U, std::uint32_t, std::uint64_t>;

template <typename SafeT, typename OvfOp, typename ThrowOp>
void test_parity(OvfOp ovf, ThrowOp thr)
{
    using basis_type = typename SafeT::basis_type;

    const std::vector<basis_type> specials {
        basis_type{0},
        static_cast<basis_type>(-0.0),
        basis_type{1},
        static_cast<basis_type>(-1),
        static_cast<basis_type>(0.5),
        basis_type{100},
        (std::numeric_limits<basis_type>::max)(),
        -(std::numeric_limits<basis_type>::max)(),
        (std::numeric_limits<basis_type>::min)(),
        std::numeric_limits<basis_type>::denorm_min(),
        std::numeric_limits<basis_type>::infinity(),
        -std::numeric_limits<basis_type>::infinity(),
        std::numeric_limits<basis_type>::quiet_NaN(),
    };

    for (const auto a : specials)
    {
        for (const auto b : specials)
        {
            const auto r {ovf(SafeT{a}, SafeT{b})};

            auto threw {false};
            auto expected {SafeT{basis_type{0}}};
            try
            {
                expected = thr(SafeT{a}, SafeT{b});
            }
            catch (const std::exception&)
            {
                threw = true;
            }

            BOOST_TEST_EQ(r.second, threw);

            if (!threw && !r.second)
            {
                const auto got_bits {std::bit_cast<bits_type<basis_type>>(static_cast<basis_type>(r.first))};
                const auto expected_bits {std::bit_cast<bits_type<basis_type>>(static_cast<basis_type>(expected))};
                BOOST_TEST_EQ(got_bits, expected_bits);
            }
        }
    }
}

template <typename SafeT>
void test_type()
{
    test_parity<SafeT>([](const SafeT a, const SafeT b) { return overflowing_add(a, b); },
                       [](const SafeT a, const SafeT b) { return a + b; });
    test_parity<SafeT>([](const SafeT a, const SafeT b) { return overflowing_sub(a, b); },
                       [](const SafeT a, const SafeT b) { return a - b; });
    test_parity<SafeT>([](const SafeT a, const SafeT b) { return overflowing_mul(a, b); },
                       [](const SafeT a, const SafeT b) { return a * b; });
    test_parity<SafeT>([](const SafeT a, const SafeT b) { return overflowing_div(a, b); },
                       [](const SafeT a, const SafeT b) { return a / b; });
}

// The family is constexpr and never throws, so overflow cases are usable in
// constant expressions where the throwing operators would fail to compile.
// (Division by zero remains outside constant evaluation, as for raw floats.)
static_assert(!overflowing_add(f32{1.0F}, f32{2.0F}).second);
static_assert(static_cast<float>(overflowing_add(f32{1.0F}, f32{2.0F}).first) == 3.0F);
static_assert(overflowing_mul(f32{(std::numeric_limits<float>::max)()}, f32{2.0F}).second);
static_assert(overflowing_div(f64{(std::numeric_limits<double>::max)()}, f64{0.5}).second);
static_assert(!overflowing_sub(f64{100.0}, f64{1.0}).second);

int main()
{
    test_type<f32>();
    test_type<f64>();

    return boost::report_errors();
}

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

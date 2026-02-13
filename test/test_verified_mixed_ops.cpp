// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <limits>
#include <compare>

#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

// =============================================================================
// Mixed arithmetic: verified_uN op uN -> uN
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_verified_op_basis_arithmetic()
{
    constexpr auto v10 = VerifiedT{BasisT{10}};
    constexpr auto v20 = VerifiedT{BasisT{20}};
    constexpr auto v5  = VerifiedT{BasisT{5}};
    constexpr auto v17 = VerifiedT{BasisT{17}};

    const auto b3 = BasisT{3};
    const auto b5 = BasisT{5};
    const auto b7 = BasisT{7};

    // verified + basis -> basis
    BOOST_TEST((v10 + b7) == BasisT{17});

    // verified - basis -> basis
    BOOST_TEST((v20 - b7) == BasisT{13});

    // verified * basis -> basis
    BOOST_TEST((v5 * b3) == BasisT{15});

    // verified / basis -> basis
    BOOST_TEST((v20 / b5) == BasisT{4});

    // verified % basis -> basis
    BOOST_TEST((v17 % b5) == BasisT{2});
}

// =============================================================================
// Mixed arithmetic: uN op verified_uN -> uN
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_basis_op_verified_arithmetic()
{
    constexpr auto v3 = VerifiedT{BasisT{3}};
    constexpr auto v5 = VerifiedT{BasisT{5}};
    constexpr auto v7 = VerifiedT{BasisT{7}};

    const auto b10 = BasisT{10};
    const auto b17 = BasisT{17};
    const auto b20 = BasisT{20};

    // basis + verified -> basis
    BOOST_TEST((b10 + v7) == BasisT{17});

    // basis - verified -> basis
    BOOST_TEST((b20 - v7) == BasisT{13});

    // basis * verified -> basis
    BOOST_TEST((b10 * v3) == BasisT{30});

    // basis / verified -> basis
    BOOST_TEST((b20 / v5) == BasisT{4});

    // basis % verified -> basis
    BOOST_TEST((b17 % v5) == BasisT{2});
}

// =============================================================================
// Asymmetric operand tests (ensures both operands are used correctly)
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_asymmetric_operands()
{
    // Use values where a op b != b op a for non-commutative ops (-, /, %)
    constexpr auto v30 = VerifiedT{BasisT{30}};
    constexpr auto v7  = VerifiedT{BasisT{7}};

    const auto b10 = BasisT{10};

    // verified - basis: 30 - 10 = 20
    BOOST_TEST((v30 - b10) == BasisT{20});

    // basis - verified: 10 - 7 = 3
    BOOST_TEST((b10 - v7) == BasisT{3});

    // verified / basis: 30 / 10 = 3
    BOOST_TEST((v30 / b10) == BasisT{3});

    // basis / verified: 10 / 7 = 1
    BOOST_TEST((b10 / v7) == BasisT{1});

    // verified % basis: 30 % 10 = 0
    BOOST_TEST((v30 % b10) == BasisT{0});

    // basis % verified: 10 % 7 = 3
    BOOST_TEST((b10 % v7) == BasisT{3});
}

// =============================================================================
// Mixed comparisons: verified_uN vs uN
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_mixed_comparisons()
{
    constexpr auto v10 = VerifiedT{BasisT{10}};
    constexpr auto v20 = VerifiedT{BasisT{20}};

    const auto b10 = BasisT{10};
    const auto b20 = BasisT{20};
    const auto b5  = BasisT{5};

    // verified == basis
    BOOST_TEST(v10 == b10);
    BOOST_TEST(!(v10 == b20));

    // verified != basis
    BOOST_TEST(v10 != b20);
    BOOST_TEST(!(v10 != b10));

    // verified < basis
    BOOST_TEST(v10 < b20);
    BOOST_TEST(!(v20 < b10));

    // verified <= basis
    BOOST_TEST(v10 <= b10);
    BOOST_TEST(v10 <= b20);
    BOOST_TEST(!(v20 <= b5));

    // verified > basis
    BOOST_TEST(v20 > b10);
    BOOST_TEST(!(v10 > b20));

    // verified >= basis
    BOOST_TEST(v10 >= b10);
    BOOST_TEST(v20 >= b10);
    BOOST_TEST(!(v10 >= b20));

    // verified <=> basis
    BOOST_TEST((v10 <=> b10) == std::strong_ordering::equal);
    BOOST_TEST((v10 <=> b20) == std::strong_ordering::less);
    BOOST_TEST((v20 <=> b10) == std::strong_ordering::greater);

    // basis == verified
    BOOST_TEST(b10 == v10);
    BOOST_TEST(!(b20 == v10));

    // basis != verified
    BOOST_TEST(b20 != v10);
    BOOST_TEST(!(b10 != v10));

    // basis < verified
    BOOST_TEST(b10 < v20);
    BOOST_TEST(!(b20 < v10));

    // basis <= verified
    BOOST_TEST(b10 <= v10);
    BOOST_TEST(b10 <= v20);

    // basis > verified
    BOOST_TEST(b20 > v10);
    BOOST_TEST(!(b10 > v20));

    // basis >= verified
    BOOST_TEST(b10 >= v10);
    BOOST_TEST(b20 >= v10);

    // basis <=> verified
    BOOST_TEST((b10 <=> v10) == std::strong_ordering::equal);
    BOOST_TEST((b10 <=> v20) == std::strong_ordering::less);
    BOOST_TEST((b20 <=> v10) == std::strong_ordering::greater);
}

// =============================================================================
// Mixed ops with bounded_uint types
// =============================================================================

void test_bounded_mixed_arithmetic()
{
    using bounded_t = bounded_uint<0u, 100u>;
    using verified_bounded_t = verified_bounded_integer<0u, 100u>;

    constexpr auto v10 = verified_bounded_t{bounded_t{u8{10}}};
    constexpr auto v20 = verified_bounded_t{bounded_t{u8{20}}};
    constexpr auto v5  = verified_bounded_t{bounded_t{u8{5}}};
    constexpr auto v7  = verified_bounded_t{bounded_t{u8{7}}};
    constexpr auto v17 = verified_bounded_t{bounded_t{u8{17}}};

    const auto b3  = bounded_t{u8{3}};
    const auto b5  = bounded_t{u8{5}};
    const auto b7  = bounded_t{u8{7}};
    const auto b10 = bounded_t{u8{10}};

    // verified + basis -> basis
    BOOST_TEST((v10 + b7) == bounded_t{u8{17}});

    // basis + verified -> basis
    BOOST_TEST((b10 + v7) == bounded_t{u8{17}});

    // verified - basis -> basis
    BOOST_TEST((v20 - b7) == bounded_t{u8{13}});

    // basis - verified -> basis
    BOOST_TEST((b10 - v5) == bounded_t{u8{5}});

    // verified * basis -> basis
    BOOST_TEST((v5 * b3) == bounded_t{u8{15}});

    // basis * verified -> basis
    BOOST_TEST((b3 * v5) == bounded_t{u8{15}});

    // verified / basis -> basis
    BOOST_TEST((v20 / b5) == bounded_t{u8{4}});

    // basis / verified -> basis
    BOOST_TEST((b10 / v5) == bounded_t{u8{2}});

    // verified % basis -> basis
    BOOST_TEST((v17 % b5) == bounded_t{u8{2}});

    // basis % verified -> basis
    BOOST_TEST((b10 % v5) == bounded_t{u8{0}});
}

void test_bounded_mixed_comparisons()
{
    using bounded_t = bounded_uint<0u, 100u>;
    using verified_bounded_t = verified_bounded_integer<0u, 100u>;

    constexpr auto v10 = verified_bounded_t{bounded_t{u8{10}}};
    constexpr auto v20 = verified_bounded_t{bounded_t{u8{20}}};

    const auto b10 = bounded_t{u8{10}};
    const auto b20 = bounded_t{u8{20}};

    // verified == basis
    BOOST_TEST(v10 == b10);
    BOOST_TEST(!(v10 == b20));

    // basis == verified
    BOOST_TEST(b10 == v10);
    BOOST_TEST(!(b20 == v10));

    // verified != basis
    BOOST_TEST(v10 != b20);
    BOOST_TEST(!(v10 != b10));

    // verified < basis
    BOOST_TEST(v10 < b20);
    BOOST_TEST(!(v20 < b10));

    // verified > basis
    BOOST_TEST(v20 > b10);
    BOOST_TEST(!(v10 > b20));

    // verified <=> basis
    BOOST_TEST((v10 <=> b10) == std::strong_ordering::equal);
    BOOST_TEST((v10 <=> b20) == std::strong_ordering::less);
    BOOST_TEST((v20 <=> b10) == std::strong_ordering::greater);

    // basis <=> verified
    BOOST_TEST((b10 <=> v10) == std::strong_ordering::equal);
    BOOST_TEST((b10 <=> v20) == std::strong_ordering::less);
    BOOST_TEST((b20 <=> v10) == std::strong_ordering::greater);
}

void test_bounded_asymmetric_operands()
{
    using bounded_t = bounded_uint<0u, 100u>;
    using verified_bounded_t = verified_bounded_integer<0u, 100u>;

    constexpr auto v30 = verified_bounded_t{bounded_t{u8{30}}};
    constexpr auto v7  = verified_bounded_t{bounded_t{u8{7}}};

    const auto b10 = bounded_t{u8{10}};

    // verified - basis: 30 - 10 = 20
    BOOST_TEST((v30 - b10) == bounded_t{u8{20}});

    // basis - verified: 10 - 7 = 3
    BOOST_TEST((b10 - v7) == bounded_t{u8{3}});

    // verified / basis: 30 / 10 = 3
    BOOST_TEST((v30 / b10) == bounded_t{u8{3}});

    // basis / verified: 10 / 7 = 1
    BOOST_TEST((b10 / v7) == bounded_t{u8{1}});

    // verified % basis: 30 % 10 = 0
    BOOST_TEST((v30 % b10) == bounded_t{u8{0}});

    // basis % verified: 10 % 7 = 3
    BOOST_TEST((b10 % v7) == bounded_t{u8{3}});
}

// =============================================================================
// Wider bounded range tests (u16, u32 basis)
// =============================================================================

void test_bounded_u16_mixed_ops()
{
    using bounded_t = bounded_uint<0u, 1000u>;
    using verified_bounded_t = verified_bounded_integer<0u, 1000u>;

    constexpr auto v100 = verified_bounded_t{bounded_t{u16{100}}};
    constexpr auto v200 = verified_bounded_t{bounded_t{u16{200}}};

    const auto b50  = bounded_t{u16{50}};
    const auto b100 = bounded_t{u16{100}};

    // Arithmetic
    BOOST_TEST((v100 + b50) == bounded_t{u16{150}});
    BOOST_TEST((b50 + v100) == bounded_t{u16{150}});
    BOOST_TEST((v200 - b50) == bounded_t{u16{150}});
    BOOST_TEST((b100 - v100) == bounded_t{u16{0}});
    BOOST_TEST((v200 / b100) == bounded_t{u16{2}});
    BOOST_TEST((b100 % v200) == bounded_t{u16{100}});

    // Comparisons
    BOOST_TEST(v100 == b100);
    BOOST_TEST(v100 < bounded_t{u16{200}});
    BOOST_TEST((v200 <=> b100) == std::strong_ordering::greater);
}

void test_bounded_u32_mixed_ops()
{
    using bounded_t = bounded_uint<0u, 100000u>;
    using verified_bounded_t = verified_bounded_integer<0u, 100000u>;

    constexpr auto v1000 = verified_bounded_t{bounded_t{u32{1000}}};
    constexpr auto v5000 = verified_bounded_t{bounded_t{u32{5000}}};

    const auto b500  = bounded_t{u32{500}};
    const auto b1000 = bounded_t{u32{1000}};

    // Arithmetic
    BOOST_TEST((v1000 + b500) == bounded_t{u32{1500}});
    BOOST_TEST((b500 + v1000) == bounded_t{u32{1500}});
    BOOST_TEST((v5000 - b500) == bounded_t{u32{4500}});
    BOOST_TEST((v5000 / b1000) == bounded_t{u32{5}});
    BOOST_TEST((b1000 / v5000) == bounded_t{u32{0}});

    // Comparisons
    BOOST_TEST(v1000 == b1000);
    BOOST_TEST(v5000 > b1000);
    BOOST_TEST(b500 < v1000);
}

// =============================================================================
// Constexpr mixed operations
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_constexpr_mixed_ops()
{
    constexpr auto v10 = VerifiedT{BasisT{10}};
    constexpr auto b5  = BasisT{5};

    // constexpr arithmetic
    constexpr auto sum  = v10 + b5;
    constexpr auto diff = v10 - b5;
    constexpr auto prod = v10 * b5;
    constexpr auto quot = v10 / b5;
    constexpr auto mod  = v10 % b5;

    static_assert(sum == BasisT{15});
    static_assert(diff == BasisT{5});
    static_assert(prod == BasisT{50});
    static_assert(quot == BasisT{2});
    static_assert(mod == BasisT{0});

    // constexpr comparisons
    static_assert(v10 == BasisT{10});
    static_assert(v10 != BasisT{5});
    static_assert(v10 > BasisT{5});
    static_assert(v10 < BasisT{20});
    static_assert((v10 <=> BasisT{10}) == std::strong_ordering::equal);
}

// =============================================================================
// Edge case: zero operations
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_zero_operations()
{
    constexpr auto v0  = VerifiedT{BasisT{0}};
    constexpr auto v42 = VerifiedT{BasisT{42}};
    constexpr auto v1  = VerifiedT{BasisT{1}};

    const auto b0  = BasisT{0};
    const auto b1  = BasisT{1};
    const auto b42 = BasisT{42};

    // Adding zero
    BOOST_TEST((v42 + b0) == BasisT{42});
    BOOST_TEST((b0 + v42) == BasisT{42});
    BOOST_TEST((v0 + b42) == BasisT{42});
    BOOST_TEST((b42 + v0) == BasisT{42});

    // Subtracting zero
    BOOST_TEST((v42 - b0) == BasisT{42});
    BOOST_TEST((b42 - v0) == BasisT{42});

    // Multiplying by zero
    BOOST_TEST((v42 * b0) == BasisT{0});
    BOOST_TEST((b0 * v42) == BasisT{0});

    // Multiplying by one
    BOOST_TEST((v42 * b1) == BasisT{42});
    BOOST_TEST((b42 * v1) == BasisT{42});
}

// =============================================================================
// Edge case: max value comparisons
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_max_value_comparisons()
{
    using underlying = detail::underlying_type_t<BasisT>;

    constexpr auto v_max = VerifiedT{BasisT{std::numeric_limits<underlying>::max()}};
    constexpr auto v0    = VerifiedT{BasisT{0}};

    const auto b_max = BasisT{std::numeric_limits<underlying>::max()};
    const auto b0    = BasisT{0};

    BOOST_TEST(v_max == b_max);
    BOOST_TEST(v0 == b0);
    BOOST_TEST(v_max > b0);
    BOOST_TEST(v0 < b_max);
    BOOST_TEST(b_max == v_max);
    BOOST_TEST(b0 < v_max);
}

// =============================================================================
// Mixed bitwise ops: verified_uN with uN
// =============================================================================

template <typename VerifiedT, typename BasisT>
void test_mixed_bitwise()
{
    constexpr auto v_ff = VerifiedT{BasisT{0xFF}};
    constexpr auto v_0f = VerifiedT{BasisT{0x0F}};
    constexpr auto v_1  = VerifiedT{BasisT{1}};
    constexpr auto v_8  = VerifiedT{BasisT{8}};

    const auto b_f0 = BasisT{0xF0};
    const auto b_0f = BasisT{0x0F};
    const auto b_3  = BasisT{3};

    // verified & basis
    BOOST_TEST((v_ff & b_0f) == BasisT{0x0F});
    BOOST_TEST((v_0f & b_f0) == BasisT{0x00});

    // basis & verified
    BOOST_TEST((b_f0 & v_0f) == BasisT{0x00});
    BOOST_TEST((b_0f & v_ff) == BasisT{0x0F});

    // verified | basis
    BOOST_TEST((v_0f | b_f0) == BasisT{0xFF});

    // basis | verified
    BOOST_TEST((b_f0 | v_0f) == BasisT{0xFF});

    // verified ^ basis
    BOOST_TEST((v_ff ^ b_0f) == BasisT{0xF0});

    // basis ^ verified
    BOOST_TEST((b_0f ^ v_ff) == BasisT{0xF0});

    // verified << basis
    BOOST_TEST((v_1 << b_3) == BasisT{8});

    // basis << verified
    BOOST_TEST((BasisT{1} << v_1) == BasisT{2});

    // verified >> basis
    BOOST_TEST((v_8 >> b_3) == BasisT{1});

    // basis >> verified
    BOOST_TEST((BasisT{8} >> v_1) == BasisT{4});
}

// =============================================================================
// Main
// =============================================================================

int main()
{
    // --- Mixed arithmetic: verified op basis ---
    test_verified_op_basis_arithmetic<verified_u8, u8>();
    test_verified_op_basis_arithmetic<verified_u16, u16>();
    test_verified_op_basis_arithmetic<verified_u32, u32>();
    test_verified_op_basis_arithmetic<verified_u64, u64>();

    // --- Mixed arithmetic: basis op verified ---
    test_basis_op_verified_arithmetic<verified_u8, u8>();
    test_basis_op_verified_arithmetic<verified_u16, u16>();
    test_basis_op_verified_arithmetic<verified_u32, u32>();
    test_basis_op_verified_arithmetic<verified_u64, u64>();

    // --- Asymmetric operand tests ---
    test_asymmetric_operands<verified_u8, u8>();
    test_asymmetric_operands<verified_u16, u16>();
    test_asymmetric_operands<verified_u32, u32>();
    test_asymmetric_operands<verified_u64, u64>();

    // --- Mixed comparisons ---
    test_mixed_comparisons<verified_u8, u8>();
    test_mixed_comparisons<verified_u16, u16>();
    test_mixed_comparisons<verified_u32, u32>();
    test_mixed_comparisons<verified_u64, u64>();

    // --- Constexpr mixed ops ---
    test_constexpr_mixed_ops<verified_u8, u8>();
    test_constexpr_mixed_ops<verified_u16, u16>();
    test_constexpr_mixed_ops<verified_u32, u32>();
    test_constexpr_mixed_ops<verified_u64, u64>();

    // --- Zero operations ---
    test_zero_operations<verified_u8, u8>();
    test_zero_operations<verified_u16, u16>();
    test_zero_operations<verified_u32, u32>();
    test_zero_operations<verified_u64, u64>();

    // --- Max value comparisons ---
    test_max_value_comparisons<verified_u8, u8>();
    test_max_value_comparisons<verified_u16, u16>();
    test_max_value_comparisons<verified_u32, u32>();
    test_max_value_comparisons<verified_u64, u64>();

    // --- Bounded integer mixed ops ---
    test_bounded_mixed_arithmetic();
    test_bounded_mixed_comparisons();
    test_bounded_asymmetric_operands();
    test_bounded_u16_mixed_ops();
    test_bounded_u32_mixed_ops();

    // --- Mixed bitwise ---
    test_mixed_bitwise<verified_u8, u8>();
    test_mixed_bitwise<verified_u16, u16>();
    test_mixed_bitwise<verified_u32, u32>();
    test_mixed_bitwise<verified_u64, u64>();

    return boost::report_errors();
}

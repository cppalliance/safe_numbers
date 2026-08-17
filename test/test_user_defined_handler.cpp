// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Verifies user defined error handlers passed through the basic_* alias
// templates: dispatch for every error kind, value semantics, noexcept
// propagation, constexpr usability, and identity of the built-in tags

// The float checks compare exact sentinel values (0.0F) produced by the
// handler, so the equality is intentional
#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <cstdint>

#endif

using namespace boost::safe_numbers;

// Returns the defined fallback value for every error: wrapping semantics for
// the integers and raw IEEE 754 results for the floats
struct wrapping_handler
{
    template <typename T>
    constexpr auto on_error(const error_kind, const T value, const char*) const noexcept -> T
    {
        return value;
    }
};

// Clamps like the saturate policy, driven by the reported kind
struct clamping_handler
{
    template <typename T>
    constexpr auto on_error(const error_kind kind, const T value, const char*) const noexcept -> T
    {
        if (kind == error_kind::overflow)
        {
            return std::numeric_limits<T>::max();
        }
        if (kind == error_kind::underflow)
        {
            return std::numeric_limits<T>::min();
        }
        return value;
    }
};

// Throws a user defined exception type carrying the library's message
struct my_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct custom_throw_handler
{
    template <typename T>
    auto on_error(const error_kind, const T, const char* msg) const -> T
    {
        throw my_error{msg};
    }
};

// Replaces every exceptional float result with zero
struct zeroing_handler
{
    template <typename T>
    constexpr auto on_error(const error_kind, const T, const char*) const noexcept -> T
    {
        return T{0};
    }
};

using wrap_u8 = basic_u8<wrapping_handler>;
using wrap_i8 = basic_i8<wrapping_handler>;
using clamp_u8 = basic_u8<clamping_handler>;
using throw_u8 = basic_u8<custom_throw_handler>;

void test_alias_identity()
{
    static_assert(std::is_same_v<basic_u8<>, u8>);
    static_assert(std::is_same_v<basic_u8<throwing>, u8>);
    static_assert(std::is_same_v<basic_u8<saturating>, sat_u8>);
    static_assert(std::is_same_v<basic_u8<strict>, strict_u8>);
    static_assert(std::is_same_v<basic_u128<saturating>, sat_u128>);
    static_assert(std::is_same_v<basic_i64<strict>, strict_i64>);
    static_assert(std::is_same_v<basic_f32<>, f32>);
    static_assert(std::is_same_v<basic_f64<saturating>, sat_f64>);

    static_assert(std::is_same_v<std::remove_cv_t<decltype(wrap_u8::error_policy)>, wrapping_handler>);
    static_assert(sizeof(wrap_u8) == sizeof(std::uint8_t));
    static_assert(std::is_trivially_copyable_v<wrap_u8>);

    BOOST_TEST(true);
}

// The wrapping handler must match the value component of overflowing_* everywhere
void test_wrapping_equivalence_u8()
{
    for (std::uint32_t i {0U}; i <= 255U; ++i)
    {
        for (std::uint32_t j {0U}; j <= 255U; ++j)
        {
            const auto a {static_cast<std::uint8_t>(i)};
            const auto b {static_cast<std::uint8_t>(j)};

            const wrap_u8 wa {a};
            const wrap_u8 wb {b};
            const u8 ua {a};
            const u8 ub {b};

            BOOST_TEST_EQ(static_cast<std::uint8_t>(wa + wb), static_cast<std::uint8_t>(overflowing_add(ua, ub).first));
            BOOST_TEST_EQ(static_cast<std::uint8_t>(wa - wb), static_cast<std::uint8_t>(overflowing_sub(ua, ub).first));
            BOOST_TEST_EQ(static_cast<std::uint8_t>(wa * wb), static_cast<std::uint8_t>(overflowing_mul(ua, ub).first));

            if (b != 0U)
            {
                BOOST_TEST_EQ(static_cast<std::uint8_t>(wa / wb), static_cast<std::uint8_t>(overflowing_div(ua, ub).first));
                BOOST_TEST_EQ(static_cast<std::uint8_t>(wa % wb), static_cast<std::uint8_t>(overflowing_mod(ua, ub).first));
            }
        }
    }
}

// The clamping handler must match the saturate policy for add/sub/mul
void test_clamping_equivalence_u8()
{
    for (std::uint32_t i {0U}; i <= 255U; ++i)
    {
        for (std::uint32_t j {0U}; j <= 255U; ++j)
        {
            const auto a {static_cast<std::uint8_t>(i)};
            const auto b {static_cast<std::uint8_t>(j)};

            const clamp_u8 ca {a};
            const clamp_u8 cb {b};
            const sat_u8 sa {a};
            const sat_u8 sb {b};

            BOOST_TEST_EQ(static_cast<std::uint8_t>(ca + cb), static_cast<std::uint8_t>(sa + sb));
            BOOST_TEST_EQ(static_cast<std::uint8_t>(ca - cb), static_cast<std::uint8_t>(sa - sb));
            BOOST_TEST_EQ(static_cast<std::uint8_t>(ca * cb), static_cast<std::uint8_t>(sa * sb));
        }
    }
}

void test_signed_edge_cases()
{
    constexpr auto min_val {std::numeric_limits<std::int8_t>::min()};

    // Two's complement wrapping at the boundaries
    constexpr wrap_i8 over {wrap_i8{std::int8_t{127}} + wrap_i8{std::int8_t{1}}};
    static_assert(static_cast<std::int8_t>(over) == min_val);

    // MIN / -1 hands the handler the wrapped value (MIN)
    constexpr wrap_i8 mdiv {wrap_i8{min_val} / wrap_i8{std::int8_t{-1}}};
    static_assert(static_cast<std::int8_t>(mdiv) == min_val);

    // MIN % -1 hands the handler the mathematical result (0)
    constexpr wrap_i8 mmod {wrap_i8{min_val} % wrap_i8{std::int8_t{-1}}};
    static_assert(static_cast<std::int8_t>(mmod) == std::int8_t{0});

    // Division by zero hands the handler the dividend
    constexpr wrap_i8 dz {wrap_i8{std::int8_t{9}} / wrap_i8{std::int8_t{0}}};
    static_assert(static_cast<std::int8_t>(dz) == std::int8_t{9});

    // Negating MIN hands the handler the wrapped value (MIN)
    constexpr wrap_i8 neg {-wrap_i8{min_val}};
    static_assert(static_cast<std::int8_t>(neg) == min_val);

    BOOST_TEST(static_cast<std::int8_t>(over) == min_val);
}

void test_increment_decrement_and_shifts()
{
    wrap_u8 inc {255U};
    ++inc;
    BOOST_TEST_EQ(static_cast<std::uint8_t>(inc), 0U);

    wrap_u8 dec {0U};
    const auto old_dec {dec--};
    BOOST_TEST_EQ(static_cast<std::uint8_t>(old_dec), 0U);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(dec), 255U);

    // Shift past the width hands the handler the mask-shifted value
    constexpr wrap_u8 shl {wrap_u8{1U} << wrap_u8{9U}};
    static_assert(static_cast<std::uint8_t>(shl) == 2U);

    BOOST_TEST(true);
}

void test_custom_exception()
{
    bool caught {false};
    try
    {
        const auto res {throw_u8{255U} + throw_u8{1U}};
        static_cast<void>(res);
    }
    catch (const my_error& e)
    {
        caught = true;
        BOOST_TEST_CSTR_EQ(e.what(), "Overflow detected in u8 addition");
    }
    BOOST_TEST(caught);

    static_assert(!noexcept(throw_u8{1U} + throw_u8{1U}));
}

void test_float_handlers()
{
    using zero_f32 = basic_f32<zeroing_handler>;

    const zero_f32 big {std::numeric_limits<float>::max()};
    BOOST_TEST_EQ(static_cast<float>(big + big), 0.0F);
    BOOST_TEST_EQ(static_cast<float>(zero_f32{1.0F} / zero_f32{0.0F}), 0.0F);
    BOOST_TEST_EQ(static_cast<float>(zero_f32{std::numeric_limits<float>::quiet_NaN()} + zero_f32{1.0F}), 0.0F);

    // The wrapping handler on floats reproduces the raw IEEE result
    using ieee_f32 = basic_f32<wrapping_handler>;
    const ieee_f32 ibig {std::numeric_limits<float>::max()};
    BOOST_TEST(std::isinf(static_cast<float>(ibig + ibig)));

    // Normal math never reaches the handler
    constexpr zero_f32 fine {zero_f32{1.5F} + zero_f32{2.0F}};
    static_assert(static_cast<float>(fine) == 3.5F);

    static_assert(noexcept(big + big));
}

void test_noexcept_and_free_functions()
{
    const wrap_u8 w {1U};

    static_assert(noexcept(w + w));
    static_assert(noexcept(w / w));
    static_assert(noexcept(w << w));
    static_assert(noexcept(++wrap_u8{0U}));

    // The named families accept handler types and preserve them
    const auto sat_res {saturating_add(wrap_u8{255U}, wrap_u8{1U})};
    static_assert(std::is_same_v<decltype(sat_res), const wrap_u8>);
    BOOST_TEST_EQ(static_cast<std::uint8_t>(sat_res), 255U);

    const auto ovf {overflowing_add(w, w)};
    static_assert(std::is_same_v<decltype(ovf.first), wrap_u8>);

    // A generic throw_exception call throws even on handler-typed operands
    bool caught {false};
    try
    {
        const auto res {add<overflow_policy::throw_exception>(wrap_u8{255U}, wrap_u8{1U})};
        static_cast<void>(res);
    }
    catch (const std::overflow_error&)
    {
        caught = true;
    }
    BOOST_TEST(caught);

    // Traits and numeric_limits admit handler types
    static_assert(detail::is_unsigned_library_type_v<wrap_u8>);
    static_assert(detail::is_library_type_v<wrap_i8>);
    static_assert(std::numeric_limits<wrap_u8>::is_specialized);
    static_assert(static_cast<std::uint8_t>(std::numeric_limits<wrap_u8>::max()) == 255U);
}

int main()
{
    test_alias_identity();
    test_wrapping_equivalence_u8();
    test_clamping_equivalence_u8();
    test_signed_edge_cases();
    test_increment_decrement_and_shifts();
    test_custom_exception();
    test_float_handlers();
    test_noexcept_and_free_functions();

    return boost::report_errors();
}

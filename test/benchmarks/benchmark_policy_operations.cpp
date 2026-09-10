// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Ablation of the response policies on the non-violating path. Every contender
// runs the same detection code; only the response differs: the default throwing
// operators, the saturating and strict aliases, two user defined handlers
// selected through basic_*, and the value returning overflowing_* and checked_*
// families (the deferred and optional responses).
//
// Two kernel shapes are measured. "scalar" is the per element shape of the host
// grid (each result passes through benchmark::DoNotOptimize). "stream" writes
// out[i] = a[i] op b[i] into an output array and only clobbers memory after the
// pass, which lets the optimizer vectorize whichever contenders it can. The
// stream shape is the one real element wise loops have, and it is free of the
// spill traffic that per element DoNotOptimize introduces.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <type_traits>

using namespace boost::safe_numbers;

// Returning the fallback value unchanged gives wrapping semantics, like Rust's
// Wrapping<T>. The handler is noexcept, so every operator on the type is too.
struct wrapping_handler
{
    template <typename T>
    constexpr auto on_error(const error_kind, const T value, const char*) const noexcept -> T
    {
        return value;
    }
};

// A handler that raises a user exception type carries a non-noexcept error
// path through user code, as the default policy does through the library.
struct custom_exception : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct throwing_handler
{
    template <typename T>
    auto on_error(const error_kind, const T, const char* msg) const -> T
    {
        throw custom_exception(msg);
    }
};

template <typename T>
struct underlying_for_bench { using type = T; };

template <typename T, auto P>
struct underlying_for_bench<detail::unsigned_integer_basis<T, P>> { using type = T; };

template <typename T, auto P>
struct underlying_for_bench<detail::signed_integer_basis<T, P>> { using type = T; };

template <typename T>
using underlying_for_bench_t = typename underlying_for_bench<T>::type;

inline constexpr std::size_t N {1'000'000};

enum class op_kind { add, sub, mul, div, mod };

inline constexpr const char* op_names[] {"add", "sub", "mul", "div", "mod"};

// Operator form used by every type carrying policy
template <op_kind K>
struct operator_form
{
    template <typename T>
    static auto apply(const T a, const T b, unsigned&)
    {
        if constexpr (K == op_kind::add) { return a + b; }
        else if constexpr (K == op_kind::sub) { return a - b; }
        else if constexpr (K == op_kind::mul) { return a * b; }
        else if constexpr (K == op_kind::div) { return a / b; }
        else { return a % b; }
    }
};

// Deferred form: the wrapped value is used and the flag is folded into an
// accumulator without a branch, as deferred_errors does on a device
template <op_kind K>
struct overflowing_form
{
    template <typename T>
    static auto apply(const T a, const T b, unsigned& flags)
    {
        const auto result {[&]()
        {
            if constexpr (K == op_kind::add) { return overflowing_add(a, b); }
            else if constexpr (K == op_kind::sub) { return overflowing_sub(a, b); }
            else if constexpr (K == op_kind::mul) { return overflowing_mul(a, b); }
            else if constexpr (K == op_kind::div) { return overflowing_div(a, b); }
            else { return overflowing_mod(a, b); }
        }()};

        flags |= static_cast<unsigned>(result.second);
        return result.first;
    }
};

// Optional form: the result is unwrapped with a defined fallback
template <op_kind K>
struct checked_form
{
    template <typename T>
    static auto apply(const T a, const T b, unsigned&)
    {
        const auto result {[&]()
        {
            if constexpr (K == op_kind::add) { return checked_add(a, b); }
            else if constexpr (K == op_kind::sub) { return checked_sub(a, b); }
            else if constexpr (K == op_kind::mul) { return checked_mul(a, b); }
            else if constexpr (K == op_kind::div) { return checked_div(a, b); }
            else { return checked_mod(a, b); }
        }()};

        return result.value_or(a);
    }
};

// Domain declared at ingress: the operands are stored as bounded values and widened
// to the operation's type per element. The widening hands the declared domain to the
// optimizer, so the default operators' checks that the domain makes impossible can be
// deleted before the loop is vectorized.
template <typename Wide>
struct bounded_ingress
{
    template <op_kind K>
    struct form
    {
        template <typename B>
        static auto apply(const B a, const B b, unsigned& flags)
        {
            const Wide x {static_cast<Wide>(a)};
            const Wide y {static_cast<Wide>(b)};
            return operator_form<K>::apply(x, y, flags);
        }
    };
};

// Control for the ingress rows: the same one-byte operands widened to the operation's
// built-in type with no checks, so the bounded ingress row can be read against a loop
// with identical memory traffic rather than against the four- or eight-byte baseline.
template <typename Wide>
struct byte_ingress
{
    template <op_kind K>
    struct form
    {
        template <typename B>
        static auto apply(const B a, const B b, unsigned& flags)
        {
            return operator_form<K>::apply(static_cast<Wide>(a), static_cast<Wide>(b), flags);
        }
    };
};

// Operands are drawn small and positive so every operation stays on the
// non-violating path for every policy; a[i] >= b[i] keeps subtraction in range.
template <typename Builtin>
std::pair<std::vector<Builtin>, std::vector<Builtin>> generate_operands()
{
    std::vector<Builtin> values;
    values.reserve(N);

    std::mt19937_64 rng(42);

    if constexpr (sizeof(Builtin) == 16U)
    {
        // The 128-bit class types are not valid distribution result types, so
        // draw 64-bit values in the same range and widen them
        std::uniform_int_distribution<std::uint64_t> dist {1U, 127U};

        for (std::size_t i {}; i < N; ++i)
        {
            values.emplace_back(static_cast<Builtin>(dist(rng)));
        }
    }
    else
    {
        using dist_type = std::conditional_t<sizeof(Builtin) == 1U,
            std::conditional_t<std::is_signed_v<Builtin>, std::int16_t, std::uint16_t>, Builtin>;
        std::uniform_int_distribution<dist_type> dist {1, static_cast<dist_type>(sizeof(Builtin) * 8U - 1U)};

        for (std::size_t i {}; i < N; ++i)
        {
            values.emplace_back(static_cast<Builtin>(dist(rng)));
        }
    }

    std::sort(values.begin(), values.end(), std::greater<>());

    std::vector<Builtin> lhs(values.begin(), values.end() - 1);
    std::vector<Builtin> rhs(values.begin() + 1, values.end());

    return {lhs, rhs};
}

template <typename T>
struct is_bounded : std::false_type {};

template <auto Min, auto Max>
struct is_bounded<bounded_uint<Min, Max>> : std::true_type {};

template <auto Min, auto Max>
struct is_bounded<bounded_int<Min, Max>> : std::true_type {};

template <typename T, typename Builtin>
std::vector<T> convert(const std::vector<Builtin>& src)
{
    std::vector<T> result;
    result.reserve(src.size());

    for (const auto value : src)
    {
        if constexpr (is_bounded<T>::value)
        {
            result.emplace_back(static_cast<detail::underlying_type_t<typename T::basis_type>>(value));
        }
        else
        {
            result.emplace_back(static_cast<T>(value));
        }
    }

    return result;
}

// Operands generated once per builtin width and converted once per contender type
template <typename Builtin>
const std::pair<std::vector<Builtin>, std::vector<Builtin>>& builtin_operands()
{
    static const auto operands {generate_operands<Builtin>()};
    return operands;
}

template <typename T, typename Builtin>
const std::pair<std::vector<T>, std::vector<T>>& operands()
{
    static const std::pair<std::vector<T>, std::vector<T>> converted {
        convert<T>(builtin_operands<Builtin>().first), convert<T>(builtin_operands<Builtin>().second)};
    return converted;
}

// The per element shape of the host grid
template <typename T, typename value_type, typename Form>
void run_scalar(benchmark::State& state)
{

    const auto& [a, b] {operands<T, value_type>()};
    const std::size_t n {a.size()};

    for (auto _ : state)
    {
        value_type counter {};
        unsigned flags {};

        for (std::size_t i {}; i < n; ++i)
        {
            auto result {static_cast<value_type>(Form::apply(a[i], b[i], flags))};
            benchmark::DoNotOptimize(result);
            counter += result;
        }

        benchmark::DoNotOptimize(counter);
        benchmark::DoNotOptimize(flags);
    }

    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(n));
}

// The element wise array shape, which the optimizer may vectorize
template <typename T, typename value_type, typename Form>
void run_stream(benchmark::State& state)
{

    const auto& [a, b] {operands<T, value_type>()};
    const std::size_t n {a.size()};
    std::vector<value_type> out(n);

    for (auto _ : state)
    {
        unsigned flags {};
        const T* pa {a.data()};
        const T* pb {b.data()};
        value_type* po {out.data()};

        for (std::size_t i {}; i < n; ++i)
        {
            po[i] = static_cast<value_type>(Form::apply(pa[i], pb[i], flags));
        }

        benchmark::DoNotOptimize(po);
        benchmark::DoNotOptimize(flags);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(n));
}

template <typename T, typename Builtin, template <op_kind> class Form, op_kind K>
void register_shapes(const std::string& base)
{
    benchmark::RegisterBenchmark("scalar_" + base, run_scalar<T, Builtin, Form<K>>);
    benchmark::RegisterBenchmark("stream_" + base, run_stream<T, Builtin, Form<K>>);
}

template <typename T, template <op_kind> class Form, typename Builtin = underlying_for_bench_t<T>>
void register_ops(const std::string& category, const int bits, const char* role)
{
    const auto name {[&](const op_kind k)
    {
        return category + "_" + std::to_string(bits) + "_" + op_names[static_cast<int>(k)] + "_" + role;
    }};

    register_shapes<T, Builtin, Form, op_kind::add>(name(op_kind::add));
    register_shapes<T, Builtin, Form, op_kind::sub>(name(op_kind::sub));
    register_shapes<T, Builtin, Form, op_kind::mul>(name(op_kind::mul));
    register_shapes<T, Builtin, Form, op_kind::div>(name(op_kind::div));
    register_shapes<T, Builtin, Form, op_kind::mod>(name(op_kind::mod));
}

// One width, every response the library offers
template <typename Builtin, template <typename> class Basic>
void register_width(const std::string& category, const int bits)
{
    register_ops<Builtin, operator_form>(category, bits, "builtin");
    register_ops<Basic<throwing>, operator_form>(category, bits, "throwing");
    register_ops<Basic<saturating>, operator_form>(category, bits, "saturating");
    register_ops<Basic<strict>, operator_form>(category, bits, "strict");
    register_ops<Basic<wrapping_handler>, operator_form>(category, bits, "wrapping_handler");
    register_ops<Basic<throwing_handler>, operator_form>(category, bits, "throwing_handler");
    register_ops<Basic<throwing>, overflowing_form>(category, bits, "overflowing");
    register_ops<Basic<throwing>, checked_form>(category, bits, "checked");

    // Operands validated into [1, bits - 1] at ingress, computed with the default
    // operators of the width; the bounded types' basis is u8 or i8 for every width here
    constexpr auto hi {sizeof(Builtin) * 8U - 1U};
    using bounded = std::conditional_t<std::is_signed_v<Builtin> || std::is_same_v<Builtin, detail::underlying_type_t<i128>>,
        bounded_int<1, static_cast<int>(hi)>, bounded_uint<1U, static_cast<unsigned>(hi)>>;
    register_ops<bounded, bounded_ingress<Basic<throwing>>::template form, Builtin>(category, bits, "throwing_bounded_ingress");

    using byte = std::conditional_t<std::is_signed_v<Builtin> || std::is_same_v<Builtin, detail::underlying_type_t<i128>>, std::int8_t, std::uint8_t>;
    register_ops<byte, byte_ingress<Builtin>::template form, Builtin>(category, bits, "builtin_byte_ingress");
}

int main(int argc, char** argv)
{
    static_assert(std::is_same_v<basic_u32<>, u32>);
    static_assert(std::is_same_v<basic_u32<saturating>, sat_u32>);
    static_assert(std::is_same_v<basic_u32<strict>, strict_u32>);

    register_width<std::uint8_t, basic_u8>("unsigned", 8);
    register_width<std::uint32_t, basic_u32>("unsigned", 32);
    register_width<std::uint64_t, basic_u64>("unsigned", 64);
    register_width<std::int32_t, basic_i32>("signed", 32);

    // The 128-bit basis types are spelled through the library so the benchmark
    // does not depend on the name of the underlying int128 type
    register_width<detail::underlying_type_t<u128>, basic_u128>("unsigned", 128);
    register_width<detail::underlying_type_t<i128>, basic_i128>("signed", 128);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    return 0;
}

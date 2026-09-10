// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Cross library comparison on the non-violating path: the built-in type,
// Boost.SafeNumbers (default throwing policy), Boost.SafeNumerics (default
// native promotion and exception policies), Microsoft SafeInt (default
// SafeIntException policy), and Intel safe-arithmetic.
//
// Intel safe-arithmetic has a different model from the other three: intervals
// are proven at compile time and a runtime check happens only where a raw value
// enters an interval type. Its per element form therefore admits both operands
// through safe::match (two range checks against the declared interval) and then
// performs the operation on interval types with no further check. Unsigned
// subtraction cannot be expressed on equal intervals without a proof that
// a >= b, so that cell is not registered.
//
// The two kernel shapes are those of benchmark_policy_operations.cpp.

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <type_traits>
#include <utility>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wsign-compare"
#  pragma clang diagnostic ignored "-Wdeprecated-copy"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wundef"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4389)
#  pragma warning(disable : 4127)
#  pragma warning(disable : 4305)
#  pragma warning(disable : 4309)
#  pragma warning(disable : 4244)
#  pragma warning(disable : 4245)
#endif

#include <boost/config.hpp>

#if defined(__GNUC__) && __GNUC__ == 11 || __GNUC__ == 12
#  ifndef BOOST_CLANG
#    define BOOST_CLANG 0
#  endif
#endif

#include <boost/safe_numerics/safe_integer.hpp>
#include <SafeInt.hpp>
#include <safe.hpp>

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

inline constexpr std::size_t N {1'000'000};

enum class op_kind { add, sub, mul, div, mod };

inline constexpr const char* op_names[] {"add", "sub", "mul", "div", "mod"};

template <op_kind K, typename T>
auto raw_op(const T a, const T b)
{
    if constexpr (K == op_kind::add) { return a + b; }
    else if constexpr (K == op_kind::sub) { return a - b; }
    else if constexpr (K == op_kind::mul) { return a * b; }
    else if constexpr (K == op_kind::div) { return a / b; }
    else { return a % b; }
}

// Operand values lie in [1, bits - 1] with a[i] >= b[i]
template <typename Builtin>
inline constexpr auto operand_max {static_cast<Builtin>(sizeof(Builtin) * 8U - 1U)};

// Contenders. Each exposes a storage type for the operand arrays and an apply
// that takes two stored operands and returns the built-in result.

template <typename Builtin>
struct builtin_contender
{
    using storage = Builtin;

    template <op_kind K>
    static Builtin apply(const storage a, const storage b)
    {
        return static_cast<Builtin>(raw_op<K>(a, b));
    }
};

template <typename Builtin>
struct safe_numbers_contender
{
    using storage = std::conditional_t<std::is_same_v<Builtin, std::uint32_t>, boost::safe_numbers::u32,
                    std::conditional_t<std::is_same_v<Builtin, std::int32_t>, boost::safe_numbers::i32,
                    std::conditional_t<std::is_same_v<Builtin, std::uint64_t>, boost::safe_numbers::u64,
                                                                               boost::safe_numbers::i64>>>;

    template <op_kind K>
    static Builtin apply(const storage a, const storage b)
    {
        return static_cast<Builtin>(raw_op<K>(a, b));
    }
};

template <typename Builtin>
struct safe_numerics_contender
{
    using storage = boost::safe_numerics::safe<Builtin>;

    template <op_kind K>
    static Builtin apply(const storage a, const storage b)
    {
        return static_cast<Builtin>(raw_op<K>(a, b));
    }
};

template <typename Builtin>
struct safeint_contender
{
    using storage = SafeInt<Builtin>;

    template <op_kind K>
    static Builtin apply(const storage a, const storage b)
    {
        return static_cast<Builtin>(raw_op<K>(a, b));
    }
};

template <typename Builtin>
struct intel_contender
{
    using storage = Builtin;
    using interval = safe::var<Builtin, safe::ival<1, operand_max<Builtin>>>;

    template <op_kind K>
    static Builtin apply(const storage a, const storage b)
    {
        // Two runtime range checks admit the operands; the operation itself is
        // proven in range at compile time and runs unchecked.
        return safe::match([](const interval x, const interval y)
        {
            return static_cast<Builtin>(raw_op<K>(x, y).unsafe_value());
        }, []() { return Builtin {}; })(a, b);
    }
};

template <typename Builtin>
std::pair<std::vector<Builtin>, std::vector<Builtin>> generate_operands()
{
    std::vector<Builtin> values;
    values.reserve(N);

    std::mt19937_64 rng(42);
    std::uniform_int_distribution<Builtin> dist {1, operand_max<Builtin>};

    for (std::size_t i {}; i < N; ++i)
    {
        values.emplace_back(dist(rng));
    }

    std::sort(values.begin(), values.end(), std::greater<>());

    std::vector<Builtin> lhs(values.begin(), values.end() - 1);
    std::vector<Builtin> rhs(values.begin() + 1, values.end());

    return {lhs, rhs};
}

template <typename Builtin>
const std::pair<std::vector<Builtin>, std::vector<Builtin>>& builtin_operands()
{
    static const auto operands {generate_operands<Builtin>()};
    return operands;
}

template <typename Storage, typename Builtin>
std::vector<Storage> convert(const std::vector<Builtin>& src)
{
    std::vector<Storage> result;
    result.reserve(src.size());

    for (const auto value : src)
    {
        result.emplace_back(static_cast<Storage>(value));
    }

    return result;
}

template <typename Contender, typename Builtin>
const std::pair<std::vector<typename Contender::storage>, std::vector<typename Contender::storage>>& operands()
{
    using storage = typename Contender::storage;
    static const std::pair<std::vector<storage>, std::vector<storage>> converted {
        convert<storage>(builtin_operands<Builtin>().first), convert<storage>(builtin_operands<Builtin>().second)};
    return converted;
}

template <typename Contender, typename Builtin, op_kind K>
void run_scalar(benchmark::State& state)
{
    const auto& [a, b] {operands<Contender, Builtin>()};
    const std::size_t n {a.size()};

    for (auto _ : state)
    {
        Builtin counter {};

        for (std::size_t i {}; i < n; ++i)
        {
            auto result {Contender::template apply<K>(a[i], b[i])};
            benchmark::DoNotOptimize(result);
            counter += result;
        }

        benchmark::DoNotOptimize(counter);
    }

    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(n));
}

template <typename Contender, typename Builtin, op_kind K>
void run_stream(benchmark::State& state)
{
    using storage = typename Contender::storage;

    const auto& [a, b] {operands<Contender, Builtin>()};
    const std::size_t n {a.size()};
    std::vector<Builtin> out(n);

    for (auto _ : state)
    {
        const storage* pa {a.data()};
        const storage* pb {b.data()};
        Builtin* po {out.data()};

        for (std::size_t i {}; i < n; ++i)
        {
            po[i] = Contender::template apply<K>(pa[i], pb[i]);
        }

        benchmark::DoNotOptimize(po);
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(n));
}

template <typename Contender, typename Builtin, op_kind K>
void register_shapes(const std::string& base)
{
    benchmark::RegisterBenchmark("scalar_" + base, run_scalar<Contender, Builtin, K>);
    benchmark::RegisterBenchmark("stream_" + base, run_stream<Contender, Builtin, K>);
}

template <typename Contender, typename Builtin, bool WithSub = true>
void register_ops(const std::string& category, const int bits, const char* role)
{
    const auto name {[&](const op_kind k)
    {
        return category + "_" + std::to_string(bits) + "_" + op_names[static_cast<int>(k)] + "_" + role;
    }};

    register_shapes<Contender, Builtin, op_kind::add>(name(op_kind::add));

    if constexpr (WithSub)
    {
        register_shapes<Contender, Builtin, op_kind::sub>(name(op_kind::sub));
    }

    register_shapes<Contender, Builtin, op_kind::mul>(name(op_kind::mul));
    register_shapes<Contender, Builtin, op_kind::div>(name(op_kind::div));
    register_shapes<Contender, Builtin, op_kind::mod>(name(op_kind::mod));
}

template <typename Builtin>
void register_width(const std::string& category, const int bits)
{
    register_ops<builtin_contender<Builtin>, Builtin>(category, bits, "builtin");
    register_ops<safe_numbers_contender<Builtin>, Builtin>(category, bits, "safe_numbers");
    register_ops<safe_numerics_contender<Builtin>, Builtin>(category, bits, "safe_numerics");
    register_ops<safeint_contender<Builtin>, Builtin>(category, bits, "safeint");
    register_ops<intel_contender<Builtin>, Builtin, std::is_signed_v<Builtin>>(category, bits, "intel");
}

int main(int argc, char** argv)
{
    register_width<std::uint32_t>("unsigned", 32);
    register_width<std::int32_t>("signed", 32);
    register_width<std::uint64_t>("unsigned", 64);
    register_width<std::int64_t>("signed", 64);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    return 0;
}

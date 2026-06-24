// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_COMPARE
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION

#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <vector>
#include <random>
#include <functional>
#include <algorithm>
#include <string>
#include <type_traits>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wsign-compare"

#  if (__clang_major__ >= 10 && !defined(__APPLE__)) || __clang_major__ >= 13
#    pragma clang diagnostic ignored "-Wdeprecated-copy"
#  endif

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
#endif

#include <boost/config.hpp>

// Even with the pragma above for -Wundef, GCC-11 and GCC-12 still fail
// This is a workaround to at least define BOOST_CLANG to a fail value for safe_numerics
#if defined(__GNUC__) && __GNUC__ == 11 || __GNUC__ == 12
#  ifndef BOOST_CLANG
#    define BOOST_CLANG 0
#  endif
#endif

#include <boost/random/uniform_int_distribution.hpp>
#include <boost/safe_numerics/safe_integer.hpp>

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

// safe_numerics emits these from template instantiations that occur in the
// benchmark body below (after the pop above), so disable them for the rest of
// this translation unit. MSVC only; GCC and Clang do not warn here.
#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(disable : 4244) // conversion, possible loss of data
#  pragma warning(disable : 4245) // signed/unsigned mismatch on conversion
#endif

using namespace boost::safe_numbers;

// Helper to extract the raw underlying type for any benchmarked type:
//   builtin          -> itself
//   safe_numbers     -> underlying basis type
//   safe_numerics    -> underlying stored type
template <typename T>
struct underlying_for_bench { using type = T; };

template <typename T>
struct underlying_for_bench<detail::signed_integer_basis<T>> { using type = T; };

template <typename T, typename PP, typename EP>
struct underlying_for_bench<boost::safe_numerics::safe<T, PP, EP>> { using type = T; };

template <typename T>
using underlying_for_bench_t = typename underlying_for_bench<T>::type;

inline constexpr std::size_t N {1'000'000};

// Builtin values are drawn small and positive and sorted in descending order so
// that add, sub, mul, div, and mod all stay on the non-overflow, non-throwing
// path for every contender, which keeps the comparison about the safety checks.
template <typename T>
std::vector<T> generate_builtin()
{
    using value_type = underlying_for_bench_t<T>;

    std::vector<T> values;
    values.reserve(N);

    std::mt19937_64 rng(42);
    boost::random::uniform_int_distribution<value_type> dist {1, sizeof(T) * 8U - 1U};

    for (std::size_t i {}; i < N; ++i)
    {
        values.emplace_back(dist(rng));
    }

    std::sort(values.begin(), values.end(), std::greater<>());

    return values;
}

template <typename T, typename U>
std::vector<T> generate_from(const std::vector<U>& src)
{
    std::vector<T> result;
    result.reserve(src.size());

    for (const auto& value : src)
    {
        result.emplace_back(static_cast<T>(value));
    }

    return result;
}

// Generated once per type and cached for the lifetime of the program so that
// data generation is never part of a timed region.
template <typename T, typename Builtin>
const std::vector<T>& data()
{
    if constexpr (std::is_same_v<T, Builtin>)
    {
        static const std::vector<T> values {generate_builtin<T>()};
        return values;
    }
    else
    {
        static const std::vector<T> values {generate_from<T>(data<Builtin, Builtin>())};
        return values;
    }
}

template <typename T, typename Op>
void run_bench(benchmark::State& state, const std::vector<T>& values, Op op)
{
    using value_type = underlying_for_bench_t<T>;

    const std::size_t n {values.size()};

    for (auto _ : state)
    {
        value_type counter {};

        for (std::size_t i {}; i + 1U < n; ++i)
        {
            auto result {static_cast<value_type>(op(values[i], values[i + 1U]))};
            benchmark::DoNotOptimize(result);
            counter += result;
        }

        benchmark::DoNotOptimize(counter);
    }

    state.SetItemsProcessed(static_cast<std::int64_t>(state.iterations()) * static_cast<std::int64_t>(n - 1U));
}

template <typename T, typename Builtin, typename Op>
void register_one(const std::string& category, int bits, const char* op_name, const char* role, Op op)
{
    const std::string name {category + "_" + std::to_string(bits) + "_" + op_name + "_" + role};
    benchmark::RegisterBenchmark(name, [op](benchmark::State& state)
    {
        run_bench<T>(state, data<T, Builtin>(), op);
    });
}

template <typename T, typename Builtin>
void register_int_ops(const std::string& category, int bits, const char* role)
{
    register_one<T, Builtin>(category, bits, "add", role, std::plus<>{});
    register_one<T, Builtin>(category, bits, "sub", role, std::minus<>{});
    register_one<T, Builtin>(category, bits, "mul", role, std::multiplies<>{});
    register_one<T, Builtin>(category, bits, "div", role, std::divides<>{});
    register_one<T, Builtin>(category, bits, "mod", role, std::modulus<>{});
}

template <typename Builtin, typename SN, typename Safe>
void register_width(const std::string& category, int bits)
{
    register_int_ops<Builtin, Builtin>(category, bits, "builtin");
    register_int_ops<SN, Builtin>(category, bits, "sn");
    register_int_ops<Safe, Builtin>(category, bits, "safe");
}

template <typename Builtin, typename SN>
void register_width_no_safe(const std::string& category, int bits)
{
    register_int_ops<Builtin, Builtin>(category, bits, "builtin");
    register_int_ops<SN, Builtin>(category, bits, "sn");
}

int main(int argc, char** argv)
{
    using boost::safe_numerics::safe;

    register_width<std::int8_t,  i8,  safe<std::int8_t>>("signed", 8);
    register_width<std::int16_t, i16, safe<std::int16_t>>("signed", 16);
    register_width<std::int32_t, i32, safe<std::int32_t>>("signed", 32);
    register_width<std::int64_t, i64, safe<std::int64_t>>("signed", 64);
    register_width_no_safe<boost::int128::int128_t, i128>("signed", 128);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    return 0;
}

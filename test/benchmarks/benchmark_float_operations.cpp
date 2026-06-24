// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/floats.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <benchmark/benchmark.h>
#include <cstdint>
#include <vector>
#include <random>
#include <functional>
#include <string>
#include <type_traits>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wdouble-promotion"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif

#include <boost/random/uniform_real_distribution.hpp>

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

using namespace boost::safe_numbers;

// Helper to extract the raw underlying type for any benchmarked type:
//   builtin          -> itself
//   safe_numbers     -> underlying basis type
template <typename T>
struct underlying_for_bench { using type = T; };

template <typename T>
struct underlying_for_bench<detail::float_basis<T>> { using type = T; };

template <typename T>
using underlying_for_bench_t = typename underlying_for_bench<T>::type;

inline constexpr std::size_t N {1'000'000};

// Values are drawn from [1, 100] so that add, sub, mul, and div all stay on the
// non-overflow path for every contender, keeping the comparison about the
// safety checks rather than about throwing.
template <typename T>
std::vector<T> generate_builtin()
{
    using value_type = underlying_for_bench_t<T>;

    std::vector<T> values;
    values.reserve(N);

    std::mt19937_64 rng(42);
    boost::random::uniform_real_distribution<value_type> dist {static_cast<value_type>(1), static_cast<value_type>(100)};

    for (std::size_t i {}; i < N; ++i)
    {
        values.emplace_back(static_cast<T>(dist(rng)));
    }

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
void register_float_ops(const std::string& category, int bits, const char* role)
{
    register_one<T, Builtin>(category, bits, "add", role, std::plus<>{});
    register_one<T, Builtin>(category, bits, "sub", role, std::minus<>{});
    register_one<T, Builtin>(category, bits, "mul", role, std::multiplies<>{});
    register_one<T, Builtin>(category, bits, "div", role, std::divides<>{});
}

template <typename Builtin, typename SN>
void register_float_width(const std::string& category, int bits)
{
    register_float_ops<Builtin, Builtin>(category, bits, "builtin");
    register_float_ops<SN, Builtin>(category, bits, "sn");
}

int main(int argc, char** argv)
{
    register_float_width<float,  f32>("float", 32);
    register_float_width<double, f64>("float", 64);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();

    return 0;
}

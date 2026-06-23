// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/floats.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <random>
#include <cstdint>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <functional>
#include <algorithm>

using namespace boost::safe_numbers;
using namespace std::chrono;

// Helper to extract the raw underlying type for any benchmarked type:
//   builtin          -> itself
//   safe_numbers     -> underlying basis type
template <typename T>
struct underlying_for_bench { using type = T; };

template <typename T>
struct underlying_for_bench<detail::float_basis<T>> { using type = T; };

template <typename T>
using underlying_for_bench_t = typename underlying_for_bench<T>::type;

inline constexpr std::size_t N {10'000'000};
inline std::mt19937_64 rng(42);

// Range chosen so consecutive add / sub never approach overflow,
// keeping the safe path on its hot branch and matching what builtin
// floats see for a fair comparison.
template <typename T>
auto generate_vector()
{
    using value_type = underlying_for_bench_t<T>;

    std::vector<T> values;
    values.reserve(N);

    boost::random::uniform_real_distribution<value_type> dist {static_cast<value_type>(1), static_cast<value_type>(100)};

    for (std::size_t i {}; i < N; ++i)
    {
        values.emplace_back(static_cast<T>(dist(rng)));
    }

    std::sort(values.begin(), values.end(), std::greater<>());

    return values;
}

template <typename T, typename U>
auto generate_vector(const std::vector<U>& values)
{
    std::vector<T> result;
    result.reserve(values.size());

    for (const auto& value : values)
    {
        result.emplace_back(static_cast<T>(value));
    }

    return result;
}

template <typename T, typename Func>
BOOST_NOINLINE
#if defined(_MSC_VER) && !defined(__clang__)
#pragma optimize("t", off)
#endif
auto
#if defined(__clang__)
__attribute__((optnone))
#elif defined(__GNUC__)
__attribute__((optimize("O0")))
#endif
benchmark_op(const std::vector<T>& values, Func op, const char* type, const char* operation)
{
    const auto t1 = steady_clock::now();

    using value_type = underlying_for_bench_t<T>;

    value_type counter {};

    for (std::size_t j {}; j < 10; ++j)
    {
        for (std::size_t i {}; i < N - 1U; ++i)
        {
            counter += static_cast<value_type>(op(values[i], values[i + 1]));
        }
    }

    const auto t2 = steady_clock::now();

    const volatile auto sink {static_cast<value_type>(counter)};
    static_cast<void>(sink);

    const auto runtime_ns = (t2 - t1) / 1ns;

    std::cerr << operation << "<" << std::left << std::setw(15) << type << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us\n";

    return runtime_ns;
}

template <typename T>
auto benchmark_addition(const std::vector<T>& values, const char* type)
{
    return benchmark_op(values, std::plus<>(), type, "add");
}

template <typename T>
auto benchmark_subtraction(const std::vector<T>& values, const char* type)
{
    return benchmark_op(values, std::minus<>(), type, "sub");
}

template <typename T>
auto benchmark_multiplication(const std::vector<T>& values, const char* type)
{
    return benchmark_op(values, std::multiplies<>(), type, "mul");
}

template <typename T>
auto benchmark_division(const std::vector<T>& values, const char* type)
{
    return benchmark_op(values, std::divides<>(), type, "div");
}

#ifdef _MSC_VER
#pragma optimize("", on)
#endif

template <typename T>
void print_runtime_ratio(T lib, T builtin)
{
    std::cout << std::setprecision(2) << std::fixed << std::setw(22)
              << "Runtime ratio: " << std::setw(3) << static_cast<double>(lib) / static_cast<double>(builtin)
              << std::endl;
}

int main()
{
    #ifdef BOOST_SAFE_NUMBERS_RUN_BENCHMARKS

    {
        std::cout << "32-bit Floats\n";
        const auto builtin_values{generate_vector<float>()};
        const auto lib_values{generate_vector<f32>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "float");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::f32");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "float");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::f32");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "float");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::f32");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "float");
        lib_runtime = benchmark_division(lib_values, "boost::sn::f32");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }
    {
        std::cout << "\n64-bit Floats\n";
        const auto builtin_values{generate_vector<double>()};
        const auto lib_values{generate_vector<f64>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "double");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::f64");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "double");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::f64");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "double");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::f64");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "double");
        lib_runtime = benchmark_division(lib_values, "boost::sn::f64");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }

    #else

    std::cerr << "Benchmarks not run" << std::endl;

    #endif

    return 1;
}

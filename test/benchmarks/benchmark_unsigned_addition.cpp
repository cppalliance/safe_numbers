// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/config.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <random>
#include <cstdint>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace boost::safe_numbers;
using namespace std::chrono;

inline constexpr std::size_t N {100'000'000};
inline std::mt19937_64 rng(42);

template <typename T>
BOOST_NOINLINE auto generate_vector()
{
    using value_type = std::conditional_t<detail::is_library_type_v<T>, detail::underlying_type_t<T>, T>;

    std::vector<T> values;
    values.reserve(N);

    boost::random::uniform_int_distribution<value_type> dist {std::numeric_limits<value_type>::min(),
                                                              std::numeric_limits<value_type>::max() / 2U - 1U};

    for (std::size_t i {}; i < N; ++i)
    {
        values.emplace_back(dist(rng));
    }

    return values;
}

template <typename T>
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
benchmark_addition(const std::vector<T>& values, const char* label)
{
    const auto t1 = steady_clock::now();

    using value_type = std::conditional_t<detail::is_library_type_v<T>, detail::underlying_type_t<T>, T>;

    value_type counter {};

    for (std::size_t j {}; j < 10; ++j)
    {
        for (std::size_t i {}; i < N - 1U; ++i)
        {
            counter += static_cast<value_type>(values[i] + values[i + 1]);
        }
    }

    const auto t2 = steady_clock::now();

    volatile auto sink {counter};

    const auto runtime_ns = (t2 - t1) / 1ns;

    std::cout << std::setw(15) << label << ": " << std::setw(13) << runtime_ns << " ns " << static_cast<std::uint64_t>(sink) << std::endl;

    return runtime_ns;
}

#ifdef _MSC_VER
#pragma optimize("", on)
#endif

template <typename T>
void print_runtime_ratio(T lib, T builtin)
{
    std::cout << std::setprecision(2) << std::fixed << std::setw(17)
              << "Runtime ratio: " << std::setw(13) << static_cast<double>(lib) / static_cast<double>(builtin)
              << std::endl;
}

int main()
{
    #ifdef BOOST_SAFE_NUMBERS_RUN_BENCHMARKS

    {
        std::cout << "8-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint8_t>()};
        const auto lib_values{generate_vector<u8>()};

        const auto builtin_runtime = benchmark_addition(builtin_values, "std::uint8_t");
        const auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }
    {
        std::cout << "\n16-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint16_t>()};
        const auto lib_values{generate_vector<u16>()};

        const auto builtin_runtime = benchmark_addition(builtin_values, "std::uint16_t");
        const auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }
    {
        std::cout << "\n32-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint32_t>()};
        const auto lib_values{generate_vector<u32>()};

        const auto builtin_runtime = benchmark_addition(builtin_values, "std::uint32_t");
        const auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }
    {
        std::cout << "\n64-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint64_t>()};
        const auto lib_values{generate_vector<u64>()};

        const auto builtin_runtime = benchmark_addition(builtin_values, "std::uint64_t");
        const auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }

    #else

    std::cerr << "Benchmarks not run" << std::endl;

    #endif

    return 1;
}

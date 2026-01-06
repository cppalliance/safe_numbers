// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/config.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <random>
#include <cstdint>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <functional>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wsign-compare"
#  pragma clang diagnostic ignored "-Woverflow"

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
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Woverflow"

#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4389)
#  pragma warning(disable : 4127)
#  pragma warning(disable : 4305)
#  pragma warning(disable : 4309)
#endif

#include <boost/random/uniform_int_distribution.hpp>

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

using namespace boost::safe_numbers;
using namespace std::chrono;

inline constexpr std::size_t N {10'000'000};
inline std::mt19937_64 rng(42);

template <typename T>
auto generate_vector()
{
    using value_type = std::conditional_t<detail::is_library_type_v<T>, detail::underlying_type_t<T>, T>;

    std::vector<T> values;
    values.reserve(N);

    boost::random::uniform_int_distribution<value_type> dist {1, static_cast<value_type>(std::sqrt(std::numeric_limits<value_type>::max()) - 1U)};

    for (std::size_t i {}; i < N; ++i)
    {
        values.emplace_back(dist(rng));
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

    using value_type = std::conditional_t<detail::is_library_type_v<T>, detail::underlying_type_t<T>, T>;

    value_type counter {};

    for (std::size_t j {}; j < 10; ++j)
    {
        for (std::size_t i {}; i < N - 1U; ++i)
        {
            counter += static_cast<value_type>(op(values[i], values[i + 1]));
        }
    }

    const auto t2 = steady_clock::now();

    volatile auto sink {counter};

    const auto runtime_ns = (t2 - t1) / 1ns;

    std::cerr << operation << "<" << std::left << std::setw(15) << type << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << static_cast<std::uint64_t>(sink) << ")\n";

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

template <typename T>
auto benchmark_modulo(const std::vector<T>& values, const char* type)
{
    return benchmark_op(values, std::modulus<>(), type, "mod");
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
        std::cout << "8-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint8_t>()};
        const auto lib_values{generate_vector<u8>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "std::uint8_t");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "std::uint8_t");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "std::uint8_t");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "std::uint8_t");
        lib_runtime = benchmark_division(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_modulo(builtin_values, "std::uint8_t");
        lib_runtime = benchmark_modulo(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }
    {
        std::cout << "\n16-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint16_t>()};
        const auto lib_values{generate_vector<u16>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "std::uint16_t");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "std::uint16_t");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "std::uint16_t");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "std::uint16_t");
        lib_runtime = benchmark_division(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_modulo(builtin_values, "std::uint16_t");
        lib_runtime = benchmark_modulo(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }
    {
        std::cout << "\n32-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint32_t>()};
        const auto lib_values{generate_vector<u32>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "std::uint32_t");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "std::uint32_t");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "std::uint32_t");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "std::uint32_t");
        lib_runtime = benchmark_division(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_modulo(builtin_values, "std::uint32_t");
        lib_runtime = benchmark_modulo(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }
    {
        std::cout << "\n64-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint64_t>()};
        const auto lib_values{generate_vector<u64>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "std::uint64_t");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "std::uint64_t");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "std::uint64_t");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "std::uint64_t");
        lib_runtime = benchmark_division(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_modulo(builtin_values, "std::uint64_t");
        lib_runtime = benchmark_modulo(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }

    #else

    std::cerr << "Benchmarks not run" << std::endl;

    #endif

    return 1;
}

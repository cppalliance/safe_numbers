// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_COMPARE
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION

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
#include <algorithm>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wsign-compare"
#  pragma clang diagnostic ignored "-Woverflow"
#  pragma clang diagnostic ignored "-Wdouble-promotion"

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
#include <boost/safe_numerics/safe_integer.hpp>

#ifdef __clang__
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

using namespace boost::safe_numbers;
using namespace std::chrono;

// Helper to extract the raw underlying type for any benchmarked type:
//   builtin          -> itself
//   safe_numbers     -> underlying basis type
//   safe_numerics    -> underlying stored type
template <typename T>
struct underlying_for_bench { using type = T; };

template <typename T>
struct underlying_for_bench<detail::unsigned_integer_basis<T>> { using type = T; };

template <typename T, typename PP, typename EP>
struct underlying_for_bench<boost::safe_numerics::safe<T, PP, EP>> { using type = T; };

template <typename T>
using underlying_for_bench_t = typename underlying_for_bench<T>::type;

inline constexpr std::size_t N {10'000'000};
inline std::mt19937_64 rng(42);

template <typename T>
auto generate_vector()
{
    using value_type = underlying_for_bench_t<T>;

    std::vector<T> values;
    values.reserve(N);

    boost::random::uniform_int_distribution<value_type> dist {1, sizeof(T) * 8U - 1U};

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

    const volatile auto sink {static_cast<std::uint64_t>(counter)};

    const auto runtime_ns = (t2 - t1) / 1ns;

    std::cerr << operation << "<" << std::left << std::setw(15) << type << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << sink << ")\n";

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
        const auto safe_values{generate_vector<boost::safe_numerics::safe<std::uint8_t>>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "std::uint8_t");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        auto safe_runtime = benchmark_addition(safe_values, "safe<uint8_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "std::uint8_t");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_subtraction(safe_values, "safe<uint8_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "std::uint8_t");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_multiplication(safe_values, "safe<uint8_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "std::uint8_t");
        lib_runtime = benchmark_division(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_division(safe_values, "safe<uint8_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_modulo(builtin_values, "std::uint8_t");
        lib_runtime = benchmark_modulo(lib_values, "boost::sn::u8");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_modulo(safe_values, "safe<uint8_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);
    }
    {
        std::cout << "\n16-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint16_t>()};
        const auto lib_values{generate_vector<u16>(builtin_values)};
        const auto safe_values{generate_vector<boost::safe_numerics::safe<std::uint16_t>>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "std::uint16_t");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        auto safe_runtime = benchmark_addition(safe_values, "safe<uint16_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "std::uint16_t");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_subtraction(safe_values, "safe<uint16_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "std::uint16_t");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_multiplication(safe_values, "safe<uint16_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "std::uint16_t");
        lib_runtime = benchmark_division(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_division(safe_values, "safe<uint16_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_modulo(builtin_values, "std::uint16_t");
        lib_runtime = benchmark_modulo(lib_values, "boost::sn::u16");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_modulo(safe_values, "safe<uint16_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);
    }
    {
        std::cout << "\n32-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint32_t>()};
        const auto lib_values{generate_vector<u32>(builtin_values)};
        const auto safe_values{generate_vector<boost::safe_numerics::safe<std::uint32_t>>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "std::uint32_t");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        auto safe_runtime = benchmark_addition(safe_values, "safe<uint32_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "std::uint32_t");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_subtraction(safe_values, "safe<uint32_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "std::uint32_t");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_multiplication(safe_values, "safe<uint32_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "std::uint32_t");
        lib_runtime = benchmark_division(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_division(safe_values, "safe<uint32_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_modulo(builtin_values, "std::uint32_t");
        lib_runtime = benchmark_modulo(lib_values, "boost::sn::u32");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_modulo(safe_values, "safe<uint32_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);
    }
    {
        std::cout << "\n64-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<std::uint64_t>()};
        const auto lib_values{generate_vector<u64>(builtin_values)};
        const auto safe_values{generate_vector<boost::safe_numerics::safe<std::uint64_t>>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "std::uint64_t");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        auto safe_runtime = benchmark_addition(safe_values, "safe<uint64_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "std::uint64_t");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_subtraction(safe_values, "safe<uint64_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "std::uint64_t");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_multiplication(safe_values, "safe<uint64_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "std::uint64_t");
        lib_runtime = benchmark_division(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_division(safe_values, "safe<uint64_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);

        builtin_runtime = benchmark_modulo(builtin_values, "std::uint64_t");
        lib_runtime = benchmark_modulo(lib_values, "boost::sn::u64");
        print_runtime_ratio(lib_runtime, builtin_runtime);
        safe_runtime = benchmark_modulo(safe_values, "safe<uint64_t>");
        print_runtime_ratio(safe_runtime, builtin_runtime);
    }
    {
        std::cout << "\n128-bit Unsigned Integers\n";
        const auto builtin_values{generate_vector<boost::int128::uint128_t>()};
        const auto lib_values{generate_vector<u128>(builtin_values)};

        auto builtin_runtime = benchmark_addition(builtin_values, "uint128_t");
        auto lib_runtime = benchmark_addition(lib_values, "boost::sn::u128");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_subtraction(builtin_values, "uint128_t");
        lib_runtime = benchmark_subtraction(lib_values, "boost::sn::u128");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_multiplication(builtin_values, "uint128_t");
        lib_runtime = benchmark_multiplication(lib_values, "boost::sn::u128");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_division(builtin_values, "uint128_t");
        lib_runtime = benchmark_division(lib_values, "boost::sn::u128");
        print_runtime_ratio(lib_runtime, builtin_runtime);

        builtin_runtime = benchmark_modulo(builtin_values, "uint128_t");
        lib_runtime = benchmark_modulo(lib_values, "boost::sn::u128");
        print_runtime_ratio(lib_runtime, builtin_runtime);
    }

    #else

    std::cerr << "Benchmarks not run" << std::endl;

    #endif

    return 1;
}

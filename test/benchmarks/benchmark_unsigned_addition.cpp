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

    std::uniform_int_distribution<value_type> dist {std::numeric_limits<value_type>::min(),
                                                    std::numeric_limits<value_type>::max() / 2U - 1U};

    for (std::size_t i {}; i < N; ++i)
    {
        values.emplace_back(dist(rng));
    }

    return values;
}

template <typename T>
BOOST_NOINLINE auto benchmark_addition(const std::vector<T>& values, const char* label)
{
    const auto t1 = steady_clock::now();

    using value_type = std::conditional_t<detail::is_library_type_v<T>, detail::underlying_type_t<T>, T>;

    volatile value_type counter {};

    for (std::size_t j {}; j < 10; ++j)
    {
        for (std::size_t i {}; i < N - 1U; ++i)
        {
            counter += static_cast<value_type>(values[i] + values[i + 1]);
        }
    }

    const auto t2 = steady_clock::now();

    const auto runtime_ns = (t2 - t1) / 1ns;

    std::cout << std::setw(15) << label << ": " << std::setw(10) << runtime_ns << " ns " << counter << std::endl;

    return runtime_ns;

}

template <typename T>
void print_runtime_ratio(T lib, T builtin)
{
    std::cout << std::setprecision(3) << std::setw(17)
              << "Runtime ratio: " << std::setw(10) << static_cast<double>(lib) / static_cast<double>(builtin)
              << std::endl;
}

int main()
{
    const auto builtin_values {generate_vector<std::uint32_t>()};
    const auto u32_values {generate_vector<u32>()};

    const auto builtin_runtime = benchmark_addition(builtin_values, "std::uint32_t");
    const auto u32_runtime = benchmark_addition(u32_values, "boost::sn::u32");
    print_runtime_ratio(u32_runtime, builtin_runtime);

    return 1;
}

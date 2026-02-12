// Copyright Robert Ramey
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_COMPARE
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION

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
#  pragma clang diagnostic ignored "-Wdivision-by-zero"

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

#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_COMPARE
#define BOOST_SAFE_NUMBERS_DETAIL_INT128_ALLOW_SIGN_CONVERSION

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <chrono>
#include <random>
#include <type_traits>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/integer.hpp>

#include <boost/safe_numerics/safe_integer.hpp>

#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/limits.hpp>
#include <boost/safe_numbers/bit.hpp>

typedef boost::safe_numerics::safe<unsigned> safe_type;

namespace boost {
namespace multiprecision {

    template <class Integer, class I2>
    typename enable_if_c<boost::safe_numerics::is_safe<Integer>::value, Integer&>::type
    multiply(Integer& result, const I2& a, const I2& b){
        return result = static_cast<Integer>(a) * static_cast<Integer>(b);
    }

    template <class Integer>
    typename enable_if_c<boost::safe_numerics::is_safe<Integer>::value, bool>::type
    bit_test(const Integer& val, unsigned index){
        Integer mask = 1;
        if (index >= sizeof(Integer) * CHAR_BIT)
            return 0;
        if (index)
            mask <<= index;
        return val & mask ? true : false;
    }

    template <class I1, class I2>
    typename enable_if_c<boost::safe_numerics::is_safe<I1>::value, I2>::type
    integer_modulus(const I1& x, I2 val){
        return x % val;
    }

    namespace detail {
        template <class T> struct double_integer;

        template <>
        struct double_integer<safe_type>{
            using type = boost::safe_numerics::safe<std::uint64_t>;
        };
    }

    template <class I1, class I2, class I3>
    typename enable_if_c<boost::safe_numerics::is_safe<I1>::value, I1>::type
    powm(const I1& a, I2 b, I3 c){
        typedef typename detail::double_integer<I1>::type double_type;

        I1 x(1), y(a);
        double_type result;

        while (b > 0){
            if (b & 1){
                multiply(result, x, y);
                x = integer_modulus(result, c);
            }
            multiply(result, y, y);
            y = integer_modulus(result, c);
            b >>= 1;
        }
        return x % c;
    }

    template <class T, class PP, class EP>
    inline unsigned
    lsb(const boost::safe_numerics::safe<T, PP, EP>& x){
        return lsb(static_cast<T>(x));
    }

} }

#include <boost/multiprecision/miller_rabin.hpp>

// -------------------------------------------------------
// Miller-Rabin implementation for boost::safe_numbers::u32
//
// The safe_numbers types use all-explicit constructors and
// same-type-only operators, so they cannot be used directly
// with the generic boost::multiprecision::miller_rabin_test.
// This standalone implementation uses checked safe_numbers
// operators (u32 and u64 arithmetic) to measure their
// overhead compared to raw unsigned and safe_numerics.
// -------------------------------------------------------

namespace safe_numbers_bench {

using u32 = boost::safe_numbers::u32;
using u64 = boost::safe_numbers::u64;

inline auto to_raw(u32 x) -> std::uint32_t { return static_cast<std::uint32_t>(x); }
inline auto to_raw(u64 x) -> std::uint64_t { return static_cast<std::uint64_t>(x); }

inline auto is_small_prime(std::uint32_t n) -> bool
{
    constexpr unsigned char p[] = {
        3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u,
        37u, 41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u,
        79u, 83u, 89u, 97u, 101u, 103u, 107u, 109u, 113u,
        127u, 131u, 137u, 139u, 149u, 151u, 157u, 163u,
        167u, 173u, 179u, 181u, 191u, 193u, 197u, 199u,
        211u, 223u, 227u};

    for (auto i : p)
    {
        if (n == i)
        {
            return true;
        }
    }

    return false;
}

inline auto check_small_factors(u32 n) -> bool
{
    // Use safe_numbers modulo for the initial reduction, then raw
    // arithmetic for the small-factor divisibility checks.

    constexpr std::uint32_t small_factors1[] = {3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u};
    constexpr std::uint32_t pp1 = 223092870u;
    auto m1 = to_raw(n % u32{pp1});
    for (auto f : small_factors1)
    {
        if (m1 % f == 0)
        {
            return false;
        }
    }

    constexpr std::uint32_t small_factors2[] = {29u, 31u, 37u, 41u, 43u, 47u};
    constexpr std::uint32_t pp2 = 2756205443u;
    m1 = to_raw(n % u32{pp2});
    for (auto f : small_factors2)
    {
        if (m1 % f == 0)
        {
            return false;
        }
    }

    constexpr std::uint32_t small_factors3[] = {53u, 59u, 61u, 67u, 71u};
    constexpr std::uint32_t pp3 = 907383479u;
    m1 = to_raw(n % u32{pp3});
    for (auto f : small_factors3)
    {
        if (m1 % f == 0)
        {
            return false;
        }
    }

    constexpr std::uint32_t small_factors4[] = {73u, 79u, 83u, 89u, 97u};
    constexpr std::uint32_t pp4 = 4132280413u;
    m1 = to_raw(n % u32{pp4});
    for (auto f : small_factors4)
    {
        if (m1 % f == 0)
        {
            return false;
        }
    }

    constexpr std::uint32_t small_factors5[6][4] = {
        {101u, 103u, 107u, 109u},
        {113u, 127u, 131u, 137u},
        {139u, 149u, 151u, 157u},
        {163u, 167u, 173u, 179u},
        {181u, 191u, 193u, 197u},
        {199u, 211u, 223u, 227u}};
    constexpr std::uint32_t pp5[6] = {
        121330189u,
        113u * 127u * 131u * 137u,
        139u * 149u * 151u * 157u,
        163u * 167u * 173u * 179u,
        181u * 191u * 193u * 197u,
        199u * 211u * 223u * 227u};

    for (std::size_t k = 0; k < 6; ++k)
    {
        m1 = to_raw(n % u32{pp5[k]});
        for (std::size_t i = 0; i < 4; ++i)
        {
            if (m1 % small_factors5[k][i] == 0)
            {
                return false;
            }
        }
    }

    return true;
}

// Modular exponentiation using checked u64 arithmetic for
// the intermediate multiply, and checked u32 for the final modulo.
inline auto powm(u32 a, u32 b, u32 c) -> u32
{
    auto x = u32{1U};
    auto y = a;
    auto raw_b = to_raw(b);

    while (raw_b > 0U)
    {
        if (raw_b & 1U)
        {
            auto rx = u64{static_cast<std::uint64_t>(to_raw(x))};
            auto ry = u64{static_cast<std::uint64_t>(to_raw(y))};
            auto product = rx * ry;
            auto rc = u64{static_cast<std::uint64_t>(to_raw(c))};
            auto mod_result = product % rc;
            x = u32{static_cast<std::uint32_t>(to_raw(mod_result))};
        }

        auto ry = u64{static_cast<std::uint64_t>(to_raw(y))};
        auto product = ry * ry;
        auto rc = u64{static_cast<std::uint64_t>(to_raw(c))};
        auto mod_result = product % rc;
        y = u32{static_cast<std::uint32_t>(to_raw(mod_result))};

        raw_b >>= 1U;
    }

    return x % c;
}

inline auto lsb(u32 x) -> unsigned
{
    return static_cast<unsigned>(boost::safe_numbers::countr_zero(x));
}

auto miller_rabin_test(u32 n, std::size_t trials) -> bool
{
    auto rn = to_raw(n);

    if (rn == 2U)
    {
        return true;
    }
    if ((rn & 1U) == 0U)
    {
        return false;
    }
    if (rn <= 227U)
    {
        return is_small_prime(rn);
    }

    if (!check_small_factors(n))
    {
        return false;
    }

    // Fermat test
    auto nm1 = n - u32{1U};
    auto x = powm(u32{228U}, nm1, n);
    if (x != u32{1U})
    {
        return false;
    }

    // Factor out powers of 2 from n-1
    auto raw_q = to_raw(nm1);
    std::size_t k = 0;
    while ((raw_q & 1U) == 0U)
    {
        raw_q >>= 1U;
        ++k;
    }
    auto q = u32{raw_q};

    // Random trials
    static std::mt19937 gen;
    std::uniform_int_distribution<std::uint32_t> dist(2U, rn - 2U);

    for (std::size_t i = 0; i < trials; ++i)
    {
        x = u32{dist(gen)};
        auto y = powm(x, q, n);
        std::size_t j = 0;

        while (true)
        {
            if (y == nm1)
            {
                break;
            }
            if (y == u32{1U})
            {
                if (j == 0)
                {
                    break;
                }
                return false;
            }
            if (++j == k)
            {
                return false;
            }
            y = powm(y, u32{2U}, n);
        }
    }

    return true;
}

} // namespace safe_numbers_bench

template <class Clock>
class stopwatch
{
    const typename Clock::time_point m_start;
public:
    stopwatch() :
        m_start(Clock::now())
    {}
    typename Clock::duration elapsed() const {
        return Clock::now() - m_start;
    }
};

template<typename T>
void test(const char * msg){
    const stopwatch<std::chrono::high_resolution_clock> c;

    unsigned count = 0;
    for (auto i = T(3); i < T(30'000'000); ++i)
    {
        bool is_prime;
        if constexpr (std::is_same_v<T, boost::safe_numbers::u32>)
        {
            is_prime = safe_numbers_bench::miller_rabin_test(i, 25);
        }
        else
        {
            is_prime = boost::multiprecision::miller_rabin_test(i, 25);
        }

        if (is_prime)
        {
            ++count;
        }
    }

    std::chrono::duration<double> time = c.elapsed();
    std::cout<< msg << ":\ntime = " << time.count();
    std::cout << "\ncount = " << count << std::endl;
}

int main()
{
    #ifdef BOOST_SAFE_NUMBERS_RUN_BENCHMARKS

    test<unsigned>("Testing type unsigned");
    test<boost::safe_numerics::safe<unsigned>>("Testing type safe<unsigned>");
    test<boost::safe_numbers::u32>("Testing type u32");

    #endif

    return 1;
}

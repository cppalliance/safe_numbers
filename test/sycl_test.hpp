//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  Shared harness for the Boost.safe_numbers SYCL tests. Each test runs an
//  operation element-wise on the SYCL device over random inputs and verifies
//  that the device results match a host recomputation. The generic runners loop
//  over every applicable width so one test file covers the whole type family.
//
//  <sycl/sycl.hpp> is included first so SYCL_EXTERNAL exists before the
//  safe_numbers headers (required by BOOST_SAFE_NUMBERS_ENABLE_SYCL).

#ifndef BOOST_SAFE_NUMBERS_TEST_SYCL_TEST_HPP
#define BOOST_SAFE_NUMBERS_TEST_SYCL_TEST_HPP

#include <sycl/sycl.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/floats.hpp>
#include <boost/safe_numbers/bit.hpp>
#include <boost/safe_numbers/numeric.hpp>
#include <boost/safe_numbers/integer_utilities.hpp>
#include <boost/safe_numbers/byte_conversions.hpp>
#include <boost/safe_numbers/device_error_reporting.hpp>

#include <iostream>
#include <limits>
#include <random>
#include <cstdint>
#include <cstdlib>
#include <type_traits>
#include <utility>

namespace sn_sycl_test {

constexpr int num_elements {20000};

// Draw an operand whose magnitude occupies only the low half of the basis value
// bits (signed operands also get a random sign). Two such operands keep add,
// sub, mul, div and mod within range under the default throwing policy, so the
// arithmetic runners never trip the overflow machinery.
template <typename SafeT>
SafeT draw_half(std::mt19937_64& rng)
{
    using basis = typename SafeT::basis_type;
    constexpr bool is_signed {std::numeric_limits<basis>::is_signed};
    constexpr int half_bits {is_signed ? (std::numeric_limits<basis>::digits / 2 - 1)
                                        : (std::numeric_limits<basis>::digits / 2)};
    const std::uint64_t mask {half_bits >= 64 ? ~std::uint64_t{0}
                                              : ((std::uint64_t{1} << half_bits) - 1)};
    const auto magnitude {static_cast<basis>(rng() & mask)};

    if constexpr (is_signed)
    {
        return SafeT{(rng() & 1U) ? static_cast<basis>(-magnitude) : magnitude};
    }
    else
    {
        return SafeT{magnitude};
    }
}

// Draw a full-range operand (for bitwise ops, comparisons, byte swaps).
template <typename SafeT>
SafeT draw_full(std::mt19937_64& rng)
{
    using basis = typename SafeT::basis_type;

    if constexpr (sizeof(basis) > 8)
    {
        using high_type = decltype(basis{}.high);
        return SafeT{basis{static_cast<high_type>(rng()), static_cast<std::uint64_t>(rng())}};
    }
    else
    {
        return SafeT{static_cast<basis>(rng())};
    }
}

// Divisor that avoids division by zero (magnitudes are already small enough to
// avoid the INT_MIN / -1 case). Callable on host and device.
template <typename SafeT>
SafeT nonzero(const SafeT b) noexcept
{
    return (b == SafeT{0}) ? SafeT{1} : b;
}

// Uniform real operand in [lo, hi] for the float types (mirrors the ranges the
// CUDA float value tests use so every op stays finite).
template <typename SafeT>
SafeT draw_real(std::mt19937_64& rng, const double lo, const double hi)
{
    using basis = typename SafeT::basis_type;
    std::uniform_real_distribution<basis> dist {static_cast<basis>(lo), static_cast<basis>(hi)};
    return SafeT{dist(rng)};
}

// Generic binary-op runner: op(a, b) is evaluated on the device for every
// element and re-evaluated on the host; any mismatch fails. op must depend only
// on device-enabled safe_numbers facilities.
template <typename SafeT, typename Gen, typename Op>
int run_binary(const char* label, Gen gen, Op op)
{
    sycl::queue q;

    SafeT* a {sycl::malloc_shared<SafeT>(num_elements, q)};
    SafeT* b {sycl::malloc_shared<SafeT>(num_elements, q)};
    SafeT* out {sycl::malloc_shared<SafeT>(num_elements, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        a[i] = gen(rng);
        b[i] = gen(rng);
    }

    q.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
    {
        const int i {static_cast<int>(idx[0])};
        out[i] = op(a[i], b[i]);
    }).wait();

    int failures {0};
    for (int i {0}; i < num_elements; ++i)
    {
        if (!(out[i] == op(a[i], b[i])))
        {
            ++failures;
        }
    }

    sycl::free(a, q);
    sycl::free(b, q);
    sycl::free(out, q);

    std::cout << label << ": " << (failures == 0 ? "PASSED" : "FAILED") << "\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Runs an arithmetic op (op-safe half-magnitude inputs) across every unsigned width.
template <typename Op>
int run_all_unsigned(const char* op_name, Op op)
{
    int rc {EXIT_SUCCESS};
    rc |= run_binary<boost::safe_numbers::u8>  ("u8 "   , &draw_half<boost::safe_numbers::u8>,   op);
    rc |= run_binary<boost::safe_numbers::u16> ("u16 "  , &draw_half<boost::safe_numbers::u16>,  op);
    rc |= run_binary<boost::safe_numbers::u32> ("u32 "  , &draw_half<boost::safe_numbers::u32>,  op);
    rc |= run_binary<boost::safe_numbers::u64> ("u64 "  , &draw_half<boost::safe_numbers::u64>,  op);
    rc |= run_binary<boost::safe_numbers::u128>("u128 " , &draw_half<boost::safe_numbers::u128>, op);
    static_cast<void>(op_name);
    return rc;
}

// Runs an arithmetic op across every signed width.
template <typename Op>
int run_all_signed(const char* op_name, Op op)
{
    int rc {EXIT_SUCCESS};
    rc |= run_binary<boost::safe_numbers::i8>  ("i8 "   , &draw_half<boost::safe_numbers::i8>,   op);
    rc |= run_binary<boost::safe_numbers::i16> ("i16 "  , &draw_half<boost::safe_numbers::i16>,  op);
    rc |= run_binary<boost::safe_numbers::i32> ("i32 "  , &draw_half<boost::safe_numbers::i32>,  op);
    rc |= run_binary<boost::safe_numbers::i64> ("i64 "  , &draw_half<boost::safe_numbers::i64>,  op);
    rc |= run_binary<boost::safe_numbers::i128>("i128 " , &draw_half<boost::safe_numbers::i128>, op);
    static_cast<void>(op_name);
    return rc;
}

// ---------------------------------------------------------------------------
// Free-function coverage (bit ops, integer utilities, numeric, byte conversions)
// ---------------------------------------------------------------------------

// Input-generation strategy for a free-function runner, chosen so the tested
// operation stays well defined for every generated value under the default
// throwing policy (mirrors the input ranges the CUDA free-function tests use).
enum class gen_kind
{
    full,            // [0, max]
    nonzero,         // [1, max]     (ilog/ilog2/ilog10, is_power_10, remove_trailing_zeros)
    below_half_max,  // [0, max / 2] (bit_ceil, next_multiple_of first operand)
    pos_10,          // [1, 10]      (lcm)
    upto_10,         // [0, 10]      (ipow base)
    upto_2,          // [0, 2]       (ipow exponent)
};

// Full-range value with the top value bit cleared, i.e. in [0, max / 2].
template <typename SafeT>
SafeT draw_below_half_max(std::mt19937_64& rng)
{
    using basis = typename SafeT::basis_type;
    return SafeT{static_cast<basis>(static_cast<basis>(draw_full<SafeT>(rng)) >> 1)};
}

// Small value in [lo, hi].
template <typename SafeT>
SafeT draw_range(std::mt19937_64& rng, const std::uint64_t lo, const std::uint64_t hi)
{
    using basis = typename SafeT::basis_type;
    return SafeT{static_cast<basis>(lo + (rng() % (hi - lo + 1)))};
}

template <gen_kind G, typename SafeT>
SafeT gen_for(std::mt19937_64& rng)
{
    if constexpr (G == gen_kind::full)                { return draw_full<SafeT>(rng); }
    else if constexpr (G == gen_kind::nonzero)        { return nonzero(draw_full<SafeT>(rng)); }
    else if constexpr (G == gen_kind::below_half_max) { return draw_below_half_max<SafeT>(rng); }
    else if constexpr (G == gen_kind::pos_10)         { return draw_range<SafeT>(rng, 1, 10); }
    else if constexpr (G == gen_kind::upto_10)        { return draw_range<SafeT>(rng, 0, 10); }
    else                                              { return draw_range<SafeT>(rng, 0, 2); }
}

// Generic unary free-function runner. The output type is deduced from the op, so
// it works whether the function returns the safe type, an int, or a bool.
template <typename InT, typename Gen, typename Op>
int run_unary(const char* label, Gen gen, Op op)
{
    using OutT = std::decay_t<decltype(op(std::declval<InT>()))>;

    sycl::queue q;
    InT*  in  {sycl::malloc_shared<InT>(num_elements, q)};
    OutT* out {sycl::malloc_shared<OutT>(num_elements, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        in[i] = gen(rng);
    }

    q.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
    {
        const int i {static_cast<int>(idx[0])};
        out[i] = op(in[i]);
    }).wait();

    int failures {0};
    for (int i {0}; i < num_elements; ++i)
    {
        if (!(out[i] == op(in[i])))
        {
            ++failures;
        }
    }

    sycl::free(in, q);
    sycl::free(out, q);

    std::cout << label << ": " << (failures == 0 ? "PASSED" : "FAILED") << "\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Generic binary free-function runner with an independent generator per operand.
template <typename InT, typename GenA, typename GenB, typename Op>
int run_binary_2(const char* label, GenA gen_a, GenB gen_b, Op op)
{
    using OutT = std::decay_t<decltype(op(std::declval<InT>(), std::declval<InT>()))>;

    sycl::queue q;
    InT*  a   {sycl::malloc_shared<InT>(num_elements, q)};
    InT*  b   {sycl::malloc_shared<InT>(num_elements, q)};
    OutT* out {sycl::malloc_shared<OutT>(num_elements, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        a[i] = gen_a(rng);
        b[i] = gen_b(rng);
    }

    q.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
    {
        const int i {static_cast<int>(idx[0])};
        out[i] = op(a[i], b[i]);
    }).wait();

    int failures {0};
    for (int i {0}; i < num_elements; ++i)
    {
        if (!(out[i] == op(a[i], b[i])))
        {
            ++failures;
        }
    }

    sycl::free(a, q);
    sycl::free(b, q);
    sycl::free(out, q);

    std::cout << label << ": " << (failures == 0 ? "PASSED" : "FAILED") << "\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Runs a unary free function across every unsigned width with the given generator.
template <gen_kind G, typename Op>
int run_unary_all_unsigned(const char* name, Op op)
{
    int rc {EXIT_SUCCESS};
    rc |= run_unary<boost::safe_numbers::u8>  (name, &gen_for<G, boost::safe_numbers::u8>,   op);
    rc |= run_unary<boost::safe_numbers::u16> (name, &gen_for<G, boost::safe_numbers::u16>,  op);
    rc |= run_unary<boost::safe_numbers::u32> (name, &gen_for<G, boost::safe_numbers::u32>,  op);
    rc |= run_unary<boost::safe_numbers::u64> (name, &gen_for<G, boost::safe_numbers::u64>,  op);
    rc |= run_unary<boost::safe_numbers::u128>(name, &gen_for<G, boost::safe_numbers::u128>, op);
    return rc;
}

// Runs a binary free function across every unsigned width, with an independent
// generator strategy for each operand.
template <gen_kind GA, gen_kind GB, typename Op>
int run_binary_all_unsigned(const char* name, Op op)
{
    int rc {EXIT_SUCCESS};
    rc |= run_binary_2<boost::safe_numbers::u8>  (name, &gen_for<GA, boost::safe_numbers::u8>,   &gen_for<GB, boost::safe_numbers::u8>,   op);
    rc |= run_binary_2<boost::safe_numbers::u16> (name, &gen_for<GA, boost::safe_numbers::u16>,  &gen_for<GB, boost::safe_numbers::u16>,  op);
    rc |= run_binary_2<boost::safe_numbers::u32> (name, &gen_for<GA, boost::safe_numbers::u32>,  &gen_for<GB, boost::safe_numbers::u32>,  op);
    rc |= run_binary_2<boost::safe_numbers::u64> (name, &gen_for<GA, boost::safe_numbers::u64>,  &gen_for<GB, boost::safe_numbers::u64>,  op);
    rc |= run_binary_2<boost::safe_numbers::u128>(name, &gen_for<GA, boost::safe_numbers::u128>, &gen_for<GB, boost::safe_numbers::u128>, op);
    return rc;
}

// Launches `launch(q)` (which submits a kernel that reports an error for at
// least one element) inside a device_error_context and asserts synchronize()
// throws the expected std exception type.
template <typename ExpectedException, typename Launch>
int expect_device_throw(const char* label, Launch launch)
{
    sycl::queue q;
    boost::safe_numbers::device_error_context ctx {q};

    launch(q);

    try
    {
        ctx.synchronize();
    }
    catch (const ExpectedException& e)
    {
        std::cout << label << ": PASSED (" << e.what() << ")\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << label << ": FAILED - wrong exception type: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    std::cerr << label << ": FAILED - no exception thrown\n";
    return EXIT_FAILURE;
}

// Fills two buffers with the given operands, applies the binary op on the
// device for every element (errors are recorded implicitly through the
// device_global buffer, exactly as on CUDA), and asserts synchronize() throws
// Expected. This is the one-liner behind the CUDA-parity error matrix.
template <typename ExpectedException, typename SafeT, typename Op>
int expect_binary_throw(const char* label, const SafeT a_val, const SafeT b_val, Op op)
{
    sycl::queue q;
    boost::safe_numbers::device_error_context ctx {q};

    constexpr int n {256};
    auto* a   {sycl::malloc_shared<SafeT>(n, q)};
    auto* b   {sycl::malloc_shared<SafeT>(n, q)};
    auto* out {sycl::malloc_shared<SafeT>(n, q)};
    for (int i {0}; i < n; ++i)
    {
        a[i] = a_val;
        b[i] = b_val;
    }

    q.parallel_for(sycl::range<1>(n), [=](sycl::id<1> idx)
    {
        const int i {static_cast<int>(idx[0])};
        out[i] = op(a[i], b[i]);
    });

    int rc {EXIT_FAILURE};
    try
    {
        ctx.synchronize();
        std::cerr << label << ": FAILED - no exception thrown\n";
    }
    catch (const ExpectedException& e)
    {
        std::cout << label << ": PASSED (" << e.what() << ")\n";
        rc = EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << label << ": FAILED - wrong exception type: " << e.what() << "\n";
    }

    sycl::free(a, q);
    sycl::free(b, q);
    sycl::free(out, q);
    return rc;
}

} // namespace sn_sycl_test

#endif // BOOST_SAFE_NUMBERS_TEST_SYCL_TEST_HPP

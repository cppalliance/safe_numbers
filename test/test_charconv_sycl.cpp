//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  Base-10 to_chars/from_chars round-trip on the SYCL device across the unsigned
//  widths. charconv pulls in Boost.Charconv, so it is kept out of the shared
//  harness and driven directly here (mirrors test_cuda_<t>_charconv.cu).

#include <sycl/sycl.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/charconv.hpp>

#include <iostream>
#include <random>
#include <cstdint>
#include <cstdlib>

namespace
{

constexpr int num_elements {20000};

template <typename T>
T draw_full(std::mt19937_64& rng)
{
    using basis = typename T::basis_type;
    if constexpr (sizeof(basis) > 8)
    {
        using high_type = decltype(basis{}.high);
        return T{basis{static_cast<high_type>(rng()), static_cast<std::uint64_t>(rng())}};
    }
    else
    {
        return T{static_cast<basis>(rng())};
    }
}

template <typename T>
int roundtrip(const char* label)
{
    sycl::queue q;
    T* in  {sycl::malloc_shared<T>(num_elements, q)};
    T* out {sycl::malloc_shared<T>(num_elements, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        in[i] = draw_full<T>(rng);
    }

    q.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
    {
        const int i {static_cast<int>(idx[0])};
        char buf[64] {};
        const auto r {boost::charconv::to_chars(buf, buf + sizeof(buf), in[i])};
        T parsed {};
        boost::charconv::from_chars(buf, r.ptr, parsed);
        out[i] = parsed;
    }).wait();

    int failures {0};
    for (int i {0}; i < num_elements; ++i)
    {
        if (!(out[i] == in[i]))
        {
            ++failures;
        }
    }

    sycl::free(in, q);
    sycl::free(out, q);

    std::cout << label << ": " << (failures == 0 ? "PASSED" : "FAILED") << "\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace

int main()
{
    using namespace boost::safe_numbers;
    int rc {EXIT_SUCCESS};
    rc |= roundtrip<u8> ("u8");
    rc |= roundtrip<u16>("u16");
    rc |= roundtrip<u32>("u32");
    rc |= roundtrip<u64>("u64");
    rc |= roundtrip<u128>("u128");
    return rc;
}

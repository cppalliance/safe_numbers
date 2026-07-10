//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  to_chars/from_chars round-trip over every base 2..36 on the SYCL device
//  across the unsigned widths (mirrors test_cuda_<t>_charconv_all_bases.cu).

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
int all_bases(const char* label)
{
    sycl::queue q;
    T*   in   {sycl::malloc_shared<T>(num_elements, q)};
    int* pass {sycl::malloc_shared<int>(num_elements, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        in[i] = draw_full<T>(rng);
    }

    q.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
    {
        const int i {static_cast<int>(idx[0])};
        int pass_count {0};
        for (int base {2}; base <= 36; ++base)
        {
            char buf[256] {};
            const auto r {boost::charconv::to_chars(buf, buf + sizeof(buf), in[i], base)};
            T parsed {};
            boost::charconv::from_chars(buf, r.ptr, parsed, base);
            if (parsed == in[i])
            {
                ++pass_count;
            }
        }
        pass[i] = pass_count;
    }).wait();

    int failures {0};
    for (int i {0}; i < num_elements; ++i)
    {
        if (pass[i] != 35)   // bases 2..36 inclusive
        {
            ++failures;
        }
    }

    sycl::free(in, q);
    sycl::free(pass, q);

    std::cout << label << ": " << (failures == 0 ? "PASSED" : "FAILED") << "\n";
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace

int main()
{
    using namespace boost::safe_numbers;
    int rc {EXIT_SUCCESS};
    rc |= all_bases<u8> ("u8");
    rc |= all_bases<u16>("u16");
    rc |= all_bases<u32>("u32");
    rc |= all_bases<u64>("u64");
    rc |= all_bases<u128>("u128");
    return rc;
}

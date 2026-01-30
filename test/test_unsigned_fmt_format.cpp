// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wconversion"
#endif

#define FMT_HEADER_ONLY

#if __has_include(<fmt/format.h>)

#include <boost/core/lightweight_test.hpp>
#include <boost/safe_numbers.hpp>
#include <boost/safe_numbers/fmt_format.hpp>
#include <fmt/format.h>

using namespace boost::safe_numbers;

template <typename T>
void test()
{
    const T x {42};

    BOOST_TEST_CSTR_EQ(fmt::format("{}", x).c_str(), "42");
    BOOST_TEST_CSTR_EQ(fmt::format("{:08x}", x).c_str(), "0000002a");
    BOOST_TEST_CSTR_EQ(fmt::format("{:#010b}", x).c_str(), "0b00101010");
}

int main()
{
    test<u8>();
    test<u16>();
    test<u32>();
    test<u64>();
    test<u128>();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif

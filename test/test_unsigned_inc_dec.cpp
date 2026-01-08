// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>
#include <limits>

#endif

#include <boost/core/lightweight_test.hpp>

using namespace boost::safe_numbers;

template <typename T>
void test_increment()
{
    using basis = typename T::basis_type;

    basis builtin {0U};
    T safe_type {builtin};

    for (std::size_t i {}; i < 20; ++i)
    {
        const auto new_builtin {++builtin};
        const auto new_safe {++safe_type};

        BOOST_TEST_EQ(new_builtin, static_cast<basis>(new_safe));
    }

    for (std::size_t i {}; i < 20; ++i)
    {
        const auto new_builtin {builtin++};
        const auto new_safe {safe_type++};

        BOOST_TEST_EQ(new_builtin, static_cast<basis>(new_safe));
    }

    BOOST_TEST_THROWS(++T{std::numeric_limits<basis>::max()}, std::overflow_error);
    BOOST_TEST_THROWS(T{std::numeric_limits<basis>::max()}++, std::overflow_error);
}

template <typename T>
void test_decrement()
{
    using basis = typename T::basis_type;

    basis builtin {100U};
    T safe_type {builtin};

    for (std::size_t i {}; i < 20; ++i)
    {
        const auto new_builtin {--builtin};
        const auto new_safe {--safe_type};

        BOOST_TEST_EQ(new_builtin, static_cast<basis>(new_safe));
    }

    for (std::size_t i {}; i < 20; ++i)
    {
        const auto new_builtin {builtin--};
        const auto new_safe {safe_type--};

        BOOST_TEST_EQ(new_builtin, static_cast<basis>(new_safe));
    }

    BOOST_TEST_THROWS(--T{0}, std::underflow_error);
    BOOST_TEST_THROWS(T{0}--, std::underflow_error);
}

int main()
{
    test_increment<u8>();
    test_increment<u16>();
    test_increment<u32>();
    test_increment<u64>();
    test_increment<u128>();

    test_decrement<u8>();
    test_decrement<u16>();
    test_decrement<u32>();
    test_decrement<u64>();
    test_decrement<u128>();

    return boost::report_errors();
}

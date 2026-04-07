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

    basis builtin {0};
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

    basis builtin {100};
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

    BOOST_TEST_THROWS(--T{std::numeric_limits<basis>::min()}, std::underflow_error);
    BOOST_TEST_THROWS(T{std::numeric_limits<basis>::min()}--, std::underflow_error);
}

template <typename T>
void test_negative_increment()
{
    using basis = typename T::basis_type;

    basis builtin {-10};
    T safe_type {builtin};

    for (std::size_t i {}; i < 20; ++i)
    {
        const auto new_builtin {++builtin};
        const auto new_safe {++safe_type};

        BOOST_TEST_EQ(new_builtin, static_cast<basis>(new_safe));
    }
}

template <typename T>
void test_negative_decrement()
{
    using basis = typename T::basis_type;

    basis builtin {10};
    T safe_type {builtin};

    for (std::size_t i {}; i < 20; ++i)
    {
        const auto new_builtin {--builtin};
        const auto new_safe {--safe_type};

        BOOST_TEST_EQ(new_builtin, static_cast<basis>(new_safe));
    }
}

int main()
{
    test_increment<i8>();
    test_increment<i16>();
    test_increment<i32>();
    test_increment<i64>();
    test_increment<i128>();

    test_decrement<i8>();
    test_decrement<i16>();
    test_decrement<i32>();
    test_decrement<i64>();
    test_decrement<i128>();

    test_negative_increment<i8>();
    test_negative_increment<i16>();
    test_negative_increment<i32>();
    test_negative_increment<i64>();
    test_negative_increment<i128>();

    test_negative_decrement<i8>();
    test_negative_decrement<i16>();
    test_negative_decrement<i32>();
    test_negative_decrement<i64>();
    test_negative_decrement<i128>();

    return boost::report_errors();
}

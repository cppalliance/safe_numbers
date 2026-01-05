// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE

import boost.safe_numbers;

#else

#include <boost/safe_numbers.hpp>

#endif

#include <type_traits>

using namespace boost::safe_numbers;

template <typename T>
void test_properties()
{
    using basis = detail::underlying_type_t<T>;

    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(std::is_trivially_destructible_v<T>);

    static_assert(std::is_standard_layout_v<T>);
    static_assert(std::has_unique_object_representations_v<T>);

    static_assert(sizeof(T) == sizeof(basis));
    static_assert(alignof(T) == alignof(basis));

    static_assert(std::is_default_constructible_v<T>);

    // This is not true, as using the default constructor does not guarantee the underlying integer
    // is initialized
    //
    // static_assert(std::is_trivially_default_constructible_v<T>);

    static_assert(std::is_nothrow_default_constructible_v<T>);

    static_assert(std::is_copy_constructible_v<T>);
    static_assert(std::is_trivially_copy_constructible_v<T>);
    static_assert(std::is_nothrow_copy_constructible_v<T>);

    static_assert(std::is_copy_assignable_v<T>);
    static_assert(std::is_trivially_copy_assignable_v<T>);
    static_assert(std::is_nothrow_copy_assignable_v<T>);

    static_assert(std::is_move_constructible_v<T>);
    static_assert(std::is_trivially_move_constructible_v<T>);

    static_assert(std::is_move_assignable_v<T>);
    static_assert(std::is_trivially_move_assignable_v<T>);
}

int main()
{
    test_properties<u8>();
    test_properties<u16>();
    test_properties<u32>();
    test_properties<u64>();

    return 0;
}

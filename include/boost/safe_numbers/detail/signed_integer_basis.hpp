// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP
#define BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/overflow_policy.hpp>
#include <boost/safe_numbers/detail/int128/bit.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/throw_exception.hpp>
#include <boost/core/bit.hpp>
#include <concepts>
#include <compare>
#include <limits>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <optional>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers::detail {

template <fundamental_signed_integral BasisType>
class signed_integer_basis
{
public:

    // This is exposed to the user so that they can convert back to built-in
    using basis_type = BasisType;

private:

    BasisType basis_ {0U};

public:

    constexpr signed_integer_basis() noexcept = default;

    explicit constexpr signed_integer_basis(const BasisType val) noexcept : basis_{val} {}

    template <typename T>
        requires std::is_same_v<T, bool>
    explicit constexpr signed_integer_basis(T) noexcept
    {
        static_assert(dependent_false<T>, "Construction from bool is not allowed");
    }
};

} // namespace boost::safe_numbers::detail

#endif //BOOST_SAFE_NUMBERS_SIGNED_INTEGER_BASIS_HPP

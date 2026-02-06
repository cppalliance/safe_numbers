// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_BIT_HPP
#define BOOST_SAFE_NUMBERS_BIT_HPP

#include <boost/safe_numbers/detail/unsigned_integer_basis.hpp>
#include <boost/safe_numbers/detail/concepts.hpp>
#include <boost/safe_numbers/detail/type_traits.hpp>
#include <boost/safe_numbers/detail/int128/bit.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/core/bit.hpp>
#include <bit>

#endif // BOOST_SAFE_NUMBERS_BUILD_MODULE

namespace boost::safe_numbers {

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto has_single_bit(const UnsignedInt x) noexcept -> bool
{
    using std::has_single_bit;
    using basis_type = typename UnsignedInt::basis_type;

    return has_single_bit(static_cast<basis_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto bit_ceil(const UnsignedInt x) noexcept -> UnsignedInt
{
    using std::bit_ceil;
    using basis_type = typename UnsignedInt::basis_type;

    return static_cast<UnsignedInt>(bit_ceil(static_cast<basis_type>(x)));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto bit_floor(const UnsignedInt x) noexcept -> UnsignedInt
{
    using std::bit_floor;
    using basis_type = typename UnsignedInt::basis_type;

    return static_cast<UnsignedInt>(bit_floor(static_cast<basis_type>(x)));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto bit_width(const UnsignedInt x) noexcept -> int
{
    using std::bit_width;
    using basis_type = typename UnsignedInt::basis_type;

    return bit_width(static_cast<basis_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto rotl(const UnsignedInt x, const int s) noexcept -> UnsignedInt
{
    using std::rotl;
    using basis_type = typename UnsignedInt::basis_type;

    return static_cast<UnsignedInt>(rotl(static_cast<basis_type>(x), s));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto rotr(const UnsignedInt x, const int s) noexcept -> UnsignedInt
{
    using std::rotr;
    using basis_type = typename UnsignedInt::basis_type;

    return static_cast<UnsignedInt>(rotr(static_cast<basis_type>(x), s));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto countl_zero(const UnsignedInt x) noexcept -> int
{
    using std::countl_zero;
    using basis_type = typename UnsignedInt::basis_type;

    return countl_zero(static_cast<basis_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto countl_one(const UnsignedInt x) noexcept -> int
{
    using std::countl_one;
    using basis_type = typename UnsignedInt::basis_type;

    return countl_one(static_cast<basis_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto countr_zero(const UnsignedInt x) noexcept -> int
{
    using std::countr_zero;
    using basis_type = typename UnsignedInt::basis_type;

    return countr_zero(static_cast<basis_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto countr_one(const UnsignedInt x) noexcept -> int
{
    using std::countr_one;
    using basis_type = typename UnsignedInt::basis_type;

    return countr_one(static_cast<basis_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto popcount(const UnsignedInt x) noexcept -> int
{
    using std::popcount;
    using basis_type = typename UnsignedInt::basis_type;

    return popcount(static_cast<basis_type>(x));
}

BOOST_SAFE_NUMBERS_EXPORT template <detail::unsigned_library_type UnsignedInt>
constexpr auto byteswap(const UnsignedInt x) noexcept -> UnsignedInt
{
    using core::byteswap;
    using basis_type = typename UnsignedInt::basis_type;

    return static_cast<UnsignedInt>(byteswap(static_cast<basis_type>(x)));
}

} // namespace boost::safe_numbers

#endif // BOOST_SAFE_NUMBERS_BIT_HPP

// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_SAFE_NUMBERS_RANDOM_HPP
#define BOOST_SAFE_NUMBERS_RANDOM_HPP

#include <boost/safe_numbers/detail/config.hpp>
#include <boost/safe_numbers/detail/int128/random.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/limits.hpp>
#include <boost/safe_numbers/literals.hpp>

#ifndef BOOST_SAFE_NUMBERS_BUILD_MODULE

#include <boost/random/uniform_int_distribution.hpp>

#endif

namespace boost::random {

template <typename SafeT>
    requires (safe_numbers::detail::is_unsigned_library_type_v<SafeT> ||
              safe_numbers::detail::is_signed_library_type_v<SafeT>)
class uniform_int_distribution<SafeT>
{
private:
    using underlying_t = safe_numbers::detail::underlying_type_t<SafeT>;

    underlying_t _min;
    underlying_t _max;
    boost::random::uniform_int_distribution<underlying_t> _dist;

public:

    class param_type
    {
    private:

        SafeT _min;
        SafeT _max;

    public:

        explicit param_type(
            const SafeT min_arg = (std::numeric_limits<SafeT>::min)(),
            const SafeT max_arg = (std::numeric_limits<SafeT>::max)())
            : _min{static_cast<underlying_t>(min_arg)}, _max{static_cast<underlying_t>(max_arg)}
        {
            BOOST_ASSERT(min_arg <= max_arg);
        }

        auto a() const -> SafeT { return _min; }
        auto b() const -> SafeT { return _max; }

        BOOST_RANDOM_DETAIL_OSTREAM_OPERATOR(os, param_type, param)
        {
            os << param._min << " " << param._max;
            return os;
        }

        BOOST_RANDOM_DETAIL_ISTREAM_OPERATOR(is, param_type, param)
        {
            auto min_in = (std::numeric_limits<SafeT>::min)();
            auto max_in = (std::numeric_limits<SafeT>::min)();

            if (is >> min_in >> std::ws >> max_in)
            {
                if (min_in <= max_in)
                {
                    param._min = min_in;
                    param._max = max_in;
                }
                else
                {
                    is.setstate(std::ios_base::failbit);
                }
            }

            return is;
        }

        BOOST_RANDOM_DETAIL_EQUALITY_OPERATOR(param_type, lhs, rhs)
        {
            return lhs._min == rhs._min && lhs._max == rhs._max;
        }

        BOOST_RANDOM_DETAIL_INEQUALITY_OPERATOR(param_type)
    };

    explicit uniform_int_distribution(
        const SafeT min_arg = (std::numeric_limits<SafeT>::min)(),
        const SafeT max_arg = (std::numeric_limits<SafeT>::max)())
        : _min{static_cast<underlying_t>(min_arg)}, _max{static_cast<underlying_t>(max_arg)}, _dist{_min, _max}
    {
        BOOST_ASSERT(min_arg <= max_arg);
    }

    explicit uniform_int_distribution(const param_type& param)
        : _min{static_cast<underlying_t>(param.a())}, _max{static_cast<underlying_t>(param.b())}, _dist{_min, _max} {}

    auto min BOOST_PREVENT_MACRO_SUBSTITUTION () const -> SafeT { return SafeT{_min}; }
    auto max BOOST_PREVENT_MACRO_SUBSTITUTION () const -> SafeT { return SafeT{_max}; }

    auto a() const -> SafeT { return SafeT{_min}; }
    auto b() const -> SafeT { return SafeT{_max}; }

    param_type param() const { return param_type{SafeT{_min}, SafeT{_max}};}

    void param(const param_type& param)
    {
        _min = static_cast<underlying_t>(param.a());
        _max = static_cast<underlying_t>(param.b());
    }

    void reset() { }

    template <typename Engine>
    auto operator()(Engine& eng) const -> SafeT
    {
        return SafeT{detail::generate_uniform_int(eng, _min, _max)};
    }

    template <typename Engine>
    auto operator()(Engine& eng, const param_type& param) const -> SafeT
    {
        return SafeT{detail::generate_uniform_int(eng, static_cast<underlying_t>(param.a()), static_cast<underlying_t>(param.b()))};
    }

    BOOST_RANDOM_DETAIL_OSTREAM_OPERATOR(os, uniform_int_distribution, ud)
    {
        os << ud.param();
        return os;
    }

    BOOST_RANDOM_DETAIL_ISTREAM_OPERATOR(is, uniform_int_distribution, ud)
    {
        param_type param;
        if (is >> param)
        {
            ud.param(param);
        }

        return is;
    }

    BOOST_RANDOM_DETAIL_EQUALITY_OPERATOR(uniform_int_distribution, lhs, rhs)
    {
        return lhs._min == rhs._min && lhs._max == rhs._max;
    }

    BOOST_RANDOM_DETAIL_INEQUALITY_OPERATOR(uniform_int_distribution)
};

} // namespace boost::random

#endif // BOOST_SAFE_NUMBERS_RANDOM_HPP

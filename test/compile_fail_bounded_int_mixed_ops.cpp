// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifdef BOOST_SAFE_NUMBERS_BUILD_MODULE
import boost.safe_numbers;
#else
#include <boost/safe_numbers.hpp>
#endif

using namespace boost::safe_numbers;

int main()
{
    constexpr bounded_int<-100, 100> a {50};
    constexpr bounded_int<-200, 200> b {50};

    // This should fail to compile: different bounds
    const auto c {a + b};
    static_cast<void>(c);
    return 0;
}

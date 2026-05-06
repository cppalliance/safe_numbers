// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/bounded_floats.hpp>
#include <boost/safe_numbers/floats.hpp>

using namespace boost::safe_numbers;

int main()
{
    bounded_float<-1.0f, 1.0f> a {f32{0.5f}};
    bounded_float<-2.0f, 2.0f> b {f32{0.5f}};

    auto c = a + b;
    (void)c;

    return 0;
}

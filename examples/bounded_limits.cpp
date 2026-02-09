// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/bounded_integers.hpp>
#include <boost/safe_numbers/limits.hpp>
#include <boost/safe_numbers/iostream.hpp>
#include <iostream>
#include <limits>

int main()
{
    using boost::safe_numbers::bounded_uint;

    using percent = bounded_uint<0u, 100u>;

    std::cout << "percent type:" << std::endl;
    std::cout << "  min:    " << std::numeric_limits<percent>::min() << std::endl;
    std::cout << "  max:    " << std::numeric_limits<percent>::max() << std::endl;
    std::cout << "  lowest: " << std::numeric_limits<percent>::lowest() << std::endl;
    std::cout << "  digits: " << std::numeric_limits<percent>::digits << std::endl;
    std::cout << std::endl;

    using port = bounded_uint<1u, 65535u>;

    std::cout << "port type:" << std::endl;
    std::cout << "  min:    " << std::numeric_limits<port>::min() << std::endl;
    std::cout << "  max:    " << std::numeric_limits<port>::max() << std::endl;
    std::cout << "  lowest: " << std::numeric_limits<port>::lowest() << std::endl;
    std::cout << "  digits: " << std::numeric_limits<port>::digits << std::endl;

    return 0;
}

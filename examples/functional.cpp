// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/safe_numbers/functional.hpp>
#include <boost/safe_numbers/unsigned_integers.hpp>
#include <boost/safe_numbers/signed_integers.hpp>
#include <boost/safe_numbers/floats.hpp>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

int main()
{
    using namespace boost::safe_numbers;

    std::cout << std::boolalpha;

    // Use a safe type as a key in an unordered container
    std::unordered_set<u32> seen {};
    seen.insert(u32{7});
    seen.insert(u32{8});
    std::cout << "seen.count(7) = " << seen.count(u32{7}) << '\n';
    std::cout << "seen.size()   = " << seen.size() << '\n';

    std::cout << '\n';

    // Safe types work as unordered_map keys too
    std::unordered_map<i64, std::string> labels {};
    labels[i64{-1}] = "minus one";
    labels[i64{42}] = "the answer";
    std::cout << "labels[-1] = " << labels[i64{-1}] << '\n';
    std::cout << "labels[42] = " << labels[i64{42}] << '\n';

    std::cout << '\n';

    // The hash of a safe value matches the hash of the underlying built-in
    const auto safe_hash {std::hash<u32>{}(u32{42})};
    const auto raw_hash {std::hash<std::uint32_t>{}(42u)};
    std::cout << "hash(u32{42}) == hash(uint32_t{42})   : " << (safe_hash == raw_hash) << '\n';

    // Floating-point values hash exactly as the built-in does
    const auto safe_f {std::hash<f64>{}(f64{3.14})};
    const auto raw_f {std::hash<double>{}(3.14)};
    std::cout << "hash(f64{3.14}) == hash(double{3.14}) : " << (safe_f == raw_f) << '\n';

    return 0;
}

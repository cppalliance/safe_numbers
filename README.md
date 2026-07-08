# Safe Numbers
A safe and fully featured replacement for C++ builtin numeric types.
**NOTE**: This is an experimental library and is subject to change without warning.

# Supported Platforms

This library requires C++20 and is tested on the following platforms:

* GCC 11 and later
* Clang 13 and later
* Visual Studio 2022 and later
* Intel OneAPI DPC++

This library also supports compilation with NVCC (tested with 12.8 and later).

# Synopsis

The Safe Numbers library provides a number of fixed width integers and floating point types:

```c++
#include <boost/safe_numbers.hpp>

namespace boost::safe_numbers {

// Unsigned integer types with specified bit width

class u8;
class u16;
class u32;
class u64;
class u128;

// Signed integer types with specified bit width

class i8;
class i16;
class i32;
class i64;
class i128;

// Binary floating point types with specified bit width

class f32;
class f64;

} // namespace boost::safe_numbers
```

These types operate much like the built-in numeric types but with far stricter behavior to enforce correctness.
This includes no implicit conversions, no mixed type operations, and throwing an exception on underflow, overflow, or other incorrect operation.
A fully featured implementation analogous to the STL is included with `<cmath>`, `<charconv>`, `<format>`, etc. support.

Using these types is straightforward and can be learned by [example](https://develop.safe-numbers.cpp.al/examples.html). 
Their usage is designed to closely resemble that of the builtin-numeric types.

# Full Documentation:

The complete documentation can be found at: https://develop.safe-numbers.cpp.al/

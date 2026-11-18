# Single header amalgamation

`extra/amalgamate.py` combines every Boost.SafeNumbers header (including the vendored copy of Boost.Int128)
into one file, `extra/single_include/boost/safe_numbers.hpp`.
The single header behaves exactly like `#include <boost/safe_numbers.hpp>`,
which makes the library easy to try out on [Compiler Explorer](https://godbolt.org) and in other single-file environments.
This follows the model of the Boost.Int128 [single header](https://github.com/cppalliance/int128/blob/develop/extra/single_include/boost/int128.hpp),
which is what its [Compiler Explorer example](https://godbolt.org/z/3q4zce41d) includes.

Unlike Boost.Int128, Boost.SafeNumbers has external dependencies, so the single header is not fully standalone.
These includes are left in place and must be found on the include path:

* `<boost/throw_exception.hpp>` (Boost.ThrowException)
* `<boost/core/bit.hpp>` and `<boost/core/detail/string_view.hpp>` (Boost.Core)
* `<boost/charconv.hpp>` (Boost.Charconv)

## Generating the single header

The script needs only Python 3 and its standard library. Run it from anywhere:

```sh
python3 extra/amalgamate.py
```

This writes `extra/single_include/boost/safe_numbers.hpp`.
The optional positional arguments `include_root top out` select a different include directory,
starting header, or output file (see `python3 extra/amalgamate.py --help`).
The script exits with an error if any internal include survives in the output.

Regenerate the header whenever anything under `include/` changes.

## Using it on Compiler Explorer

1. Choose a C++ compiler such as x86-64 GCC or x86-64 Clang, and pass `-std=c++20` or later.
2. Open **Libraries** and add **Boost** version **1.89.0 or later**. Older Boost releases fail to compile the single header
   because the vendored Boost.Int128 calls a Boost.Charconv internal (`detail::to_chars_128integer_impl`) whose signature changed in 1.89.0.
3. Include the single header by URL:

```cpp
#include <https://raw.githubusercontent.com/cppalliance/safe_numbers/refs/heads/develop/extra/single_include/boost/safe_numbers.hpp>
#include <iostream>
#include <stdexcept>

int main()
{
    using namespace boost::safe_numbers;

    const u32 a {4'000'000'000U};
    const u32 b {500'000'000U};

    try
    {
        const u32 c {a + b};
        std::cout << "a + b = " << c << '\n';
    }
    catch (const std::overflow_error& e)
    {
        std::cout << "Caught: " << e.what() << '\n';
    }

    const sat_u32 sa {4'000'000'000U};
    const sat_u32 sb {500'000'000U};
    std::cout << "saturating a + b = " << sa + sb << '\n';

    const u128 big {u128{1U} << u128{100U}};
    std::cout << "2^100 = " << big << '\n';

    return 0;
}
```

Output:

```
Caught: Overflow detected in u32 addition
saturating a + b = 4294967295
2^100 = 1267650600228229401496703205376
```

The Compiler Explorer website fetches an `#include <https://...>` header in your browser and sends its contents along with your source,
which is the same mechanism the Boost.Int128 example above uses.
This happens client side only; the Compiler Explorer REST API does not support URL includes, so pass the header in its `files` array there instead.
The URL above only resolves once the generated header has been committed to `develop`.
To share a link that keeps working after the library changes, replace `refs/heads/develop` with a commit SHA.
To try local changes, generate the header and include it from another raw URL, such as a branch on your fork.

The single header has been compiled and run on Compiler Explorer (supplied through the API `files` array) with:

* x86-64 GCC 16.2: Boost 1.89.0, 1.90.0, 1.91.0, and 1.92.0 with `-std=c++20`; Boost 1.92.0 with `-std=c++23`
* x86-64 GCC 11.5: Boost 1.89.0 with `-std=c++20`
* x86-64 Clang 22.1: Boost 1.89.0 and 1.92.0 with `-std=c++20`; Boost 1.92.0 with `-std=c++23`

### Limitations on Compiler Explorer

* **MSVC is not supported.** Compiler Explorer does not provide the Boost library to its Windows compilers,
  so the build fails with `Cannot open include file: 'boost/throw_exception.hpp'`.
* **Floating point `to_chars` and `from_chars` do not link.** Compiler Explorer ships Boost as headers only,
  and Boost.Charconv implements its floating point conversions in a compiled library.
  Calling `boost::charconv::to_chars` or `from_chars` with `f32` or `f64` compiles,
  but executing (or linking to binary) fails with `undefined reference to boost::charconv::to_chars(...)`.
  The integer overloads are header only and work, as do `std::format` and stream output of `f32` and `f64`.
* **Opt-in headers are not included.** `boost/safe_numbers/random.hpp` and `boost/safe_numbers/fmt_format.hpp`
  are not part of `<boost/safe_numbers.hpp>`, so they are not part of the single header either.

## Using it locally

The single header also works outside Compiler Explorer.
Put `extra/single_include` and a Boost installation (1.89.0 or later) on the include path:

```sh
g++ -std=c++20 -I path/to/safe_numbers/extra/single_include -I path/to/boost main.cpp
```

with `main.cpp` containing `#include <boost/safe_numbers.hpp>`.

# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

# Built only when BOOST_SAFE_NUMBERS_BUILD_BENCHMARKS is ON (see the root
# CMakeLists.txt). Google Benchmark is pulled in with FetchContent so no system
# install is required, which keeps local and CI builds identical.

include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(googlebenchmark)

set(BOOST_SAFE_NUMBERS_BENCHMARK_TARGETS
    benchmark_unsigned_operations
    benchmark_signed_operations
    benchmark_float_operations
)

foreach(bench ${BOOST_SAFE_NUMBERS_BENCHMARK_TARGETS})

    add_executable(${bench} ${bench}.cpp)

    target_link_libraries(${bench}
        PRIVATE
        Boost::safe_numbers
        Boost::safe_numerics
        Boost::random
        benchmark::benchmark
    )

    target_compile_features(${bench} PRIVATE cxx_std_20)

endforeach()

# Copyright 2026 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

# Built only when BOOST_SAFE_NUMBERS_BUILD_BENCHMARKS is ON (see the root
# CMakeLists.txt). The host benchmarks use Google Benchmark, pulled in with
# FetchContent so no system install is required. The CUDA and SYCL device
# benchmarks time kernels with the backend's own event machinery instead, so
# under BOOST_SAFE_NUMBERS_ENABLE_CUDA / BOOST_SAFE_NUMBERS_ENABLE_SYCL only
# the matching device benchmark is built (mirroring test/CMakeLists.txt).

if(BOOST_SAFE_NUMBERS_ENABLE_CUDA)

    message(STATUS "Building the Boost.safe_numbers CUDA benchmark")

    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)
    set(CMAKE_CUDA_EXTENSIONS OFF)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --expt-relaxed-constexpr")

    add_executable(benchmark_cuda_operations benchmark_cuda_operations.cu)
    target_link_libraries(benchmark_cuda_operations PRIVATE Boost::safe_numbers CUDA::cudart)
    target_compile_definitions(benchmark_cuda_operations PRIVATE BOOST_SAFE_NUMBERS_ENABLE_CUDA=1)
    target_compile_features(benchmark_cuda_operations PRIVATE cuda_std_20)

    return()

elseif(BOOST_SAFE_NUMBERS_ENABLE_SYCL)

    message(STATUS "Building the Boost.safe_numbers SYCL benchmark")

    # Configure with -DCMAKE_CXX_COMPILER=icpx. -fsycl must be on the link line
    # too (it bundles the device image); without it the executable segfaults at
    # kernel submission. See test/CMakeLists.txt for the range rounding define.
    #
    # icpx defaults to -ffp-model=fast, which grants the optimizer floating
    # point associativity: the builtin baseline chains then fold algebraically
    # and the comparison is meaningless (and no longer bit-identical). Pin the
    # precise model so both contenders run IEEE arithmetic, as nvcc does.
    #
    # The benchmark packs a couple of hundred kernels into one translation
    # unit. At -O3 the resulting monolithic device image trips a JIT symbol
    # materialization failure (old_llvm.umul.with.overflow.*) on the OpenCL
    # CPU backend, so split the device code per kernel; the split also has to
    # be on the link line, where the post-link tool actually runs.
    add_executable(benchmark_sycl_operations benchmark_sycl_operations.cpp)
    target_link_libraries(benchmark_sycl_operations PRIVATE Boost::safe_numbers sycl)
    target_compile_definitions(benchmark_sycl_operations PRIVATE BOOST_SAFE_NUMBERS_ENABLE_SYCL=1 __SYCL_DISABLE_PARALLEL_FOR_RANGE_ROUNDING__=1)
    target_compile_options(benchmark_sycl_operations PRIVATE -fsycl -ffp-model=precise -fsycl-device-code-split=per_kernel)
    target_link_options(benchmark_sycl_operations PRIVATE -fsycl -fsycl-device-code-split=per_kernel)
    target_compile_features(benchmark_sycl_operations PRIVATE cxx_std_20)

    return()

endif()

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

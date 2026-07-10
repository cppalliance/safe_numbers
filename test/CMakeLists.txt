# Copyright 2018, 2019 Peter Dimov
# Copyright 2023 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

include(BoostTestJamfile OPTIONAL RESULT_VARIABLE HAVE_BOOST_TEST)

if(HAVE_BOOST_TEST)

    if(BOOST_SAFE_NUMBERS_ENABLE_CUDA)

        message(STATUS "Building Boost.safe_numbers with CUDA")

        enable_language(CUDA)
        find_package(CUDAToolkit REQUIRED)
        set(CMAKE_CUDA_EXTENSIONS OFF)
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --expt-relaxed-constexpr")

        enable_testing()

        boost_test_jamfile(FILE cuda_jamfile LINK_LIBRARIES Boost::safe_numbers Boost::random CUDA::cudart COMPILE_DEFINITIONS BOOST_SAFE_NUMBERS_ENABLE_CUDA=1)

    elseif(BOOST_SAFE_NUMBERS_ENABLE_SYCL)

        message(STATUS "Building Boost.safe_numbers with SYCL")

        set(CMAKE_CXX_COMPILER "icpx")
        set(CMAKE_C_COMPILER "icx")

        # -fsycl must be on the link line too (it bundles the device image); COMPILE_OPTIONS
        # only reaches the compile step, so add it to the linker flags as well. Without this
        # every test links with no device image and segfaults at kernel submission.
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsycl")

        enable_testing()

        # __SYCL_DISABLE_PARALLEL_FOR_RANGE_ROUNDING__: the DPC++ range-rounding wrapper
        # (used for a plain parallel_for over a non-round range) cannot materialize the
        # small-integer llvm.umul.with.overflow intrinsic that unsigned multiplication of
        # u8/u16/u32 lowers to, and aborts the kernel at JIT time. Disabling range rounding
        # avoids the wrapper. See the note in doc/.../sycl.adoc.
        boost_test_jamfile(FILE sycl_jamfile LINK_LIBRARIES Boost::safe_numbers Boost::charconv sycl COMPILE_DEFINITIONS BOOST_SAFE_NUMBERS_ENABLE_SYCL=1 __SYCL_DISABLE_PARALLEL_FOR_RANGE_ROUNDING__=1 COMPILE_OPTIONS -fsycl)

    else()

        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64|i386|i686|x86" AND NOT MINGW)
            boost_test_jamfile(FILE Jamfile LINK_LIBRARIES Boost::safe_numbers Boost::config Boost::multiprecision Boost::safe_numerics Boost::throw_exception Boost::exception Boost::stacktrace_from_exception)
        else()
            boost_test_jamfile(FILE Jamfile LINK_LIBRARIES Boost::safe_numbers Boost::config Boost::multiprecision Boost::safe_numerics Boost::throw_exception Boost::exception)
        endif()

    endif()

endif()

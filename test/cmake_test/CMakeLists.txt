# Copyright 2018, 2019 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)

project(cmake_test LANGUAGES CXX)

if(BOOST_CI_INSTALL_TEST)

    # Workaround until B2 also installs targets for header-only libs
    if(BOOST_CI_INSTALLED_BY STREQUAL "B2")
        find_package(Boost COMPONENTS headers charconv random REQUIRED)
        add_library(boost_safe_numbers INTERFACE IMPORTED GLOBAL)
        target_link_libraries(boost_safe_numbers INTERFACE Boost::headers Boost::charconv Boost::random)
        add_library(Boost::safe_numbers ALIAS boost_safe_numbers)
    else()
        find_package(Boost COMPONENTS safe_numbers REQUIRED)
    endif()

else()

    add_subdirectory(../.. boostorg/safe_numbers)

    set(deps

            # Primary dependencies

            charconv
            config
            core
            multiprecision
            random
            safe_numerics
            throw_exception

            # Secondary dependencies

            assert # (from charconv)
            compat # (from random)
            integer # (from multiprecision)
            lexical_cast # (from multiprecision)
            math # (from multiprecision)
            predef # (from multiprecision)
            dynamic_bitset # (from random)
            io # (from random)
            system # (from random)
            type_traits # (from random)
            utility # (from random)
            concept_check # (from safe_numerics)
            logic # (from safe_numerics)
            mp11 # (from safe_numerics)
            preprocessor # (from concept_check)
            container_hash # (from dynamic_bitset)
            move # (from dynamic_bitset)
            container # (from lexical_cast)
            variant2 # (from system)
            winapi # (from system)
            intrusive # (from container)
            describe # (from container_hash)

    )

    foreach(dep IN LISTS deps)

        add_subdirectory(../../../${dep} boostorg/${dep})

    endforeach()

endif()
    
add_executable(quick ../quick.cpp)
target_link_libraries(quick Boost::safe_numbers)
# B2-installed config does not propagate the C++20 requirement, so set it explicitly here.
target_compile_features(quick PRIVATE cxx_std_20)

enable_testing()
add_test(quick quick)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)

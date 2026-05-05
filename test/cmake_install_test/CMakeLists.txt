# Copyright 2018, 2019 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)

project(cmake_install_test LANGUAGES CXX)

if(BOOST_CI_INSTALL_TEST)
    # find_package(Boost COMPONENTS boost_safe_numbers has been removed
    find_package(boost_safe_numbers REQUIRED)
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

enable_testing()
add_test(quick quick)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)

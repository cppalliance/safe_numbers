# Copyright 2025 Matt Borland
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.12...3.20)

project(boost_safe_numbers VERSION "${BOOST_SUPERPROJECT_VERSION}" LANGUAGES CXX)

add_library(boost_safe_numbers INTERFACE)
add_library(Boost::safe_numbers ALIAS boost_safe_numbers)

target_compile_features(boost_safe_numbers INTERFACE cxx_std_20)

target_include_directories(boost_safe_numbers INTERFACE include)

target_link_libraries(boost_safe_numbers
        INTERFACE
        Boost::assert
        Boost::charconv
        Boost::config
        Boost::throw_exception
)

if(BUILD_TESTING)

    add_subdirectory(test)

endif()
